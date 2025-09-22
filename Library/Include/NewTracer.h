#pragma once

#include <unordered_map>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <cstdint>

namespace NewTracer {

    struct Info {         // 32Bit 64Bit 
        void* ptr;        // 4     8 
        size_t size;      // 4     4 
        const char* file; // 4     8 
        int line;         // 4     4 

        Info(void* p = nullptr, size_t s = 0, const char* f = nullptr, int l = 0)
            : ptr(p), size(s), file(f), line(l) {
        }

        Info& operator=(const Info& other) {
            if (this != &other) {
                ptr = other.ptr;
                size = other.size;
                file = other.file;
                line = other.line;
            }
            return *this;
        }
    };

    class Carray {
    private:
        Info* _data;
        size_t _size;
        size_t _capacity;

    public:
        inline bool empty() const { return _size == 0; }
        inline size_t size() const { return _size; }
        inline size_t capacity() const { return _capacity; }

    public:
        Carray();
        Carray(size_t capacity);
        Carray(const Carray&) = delete;
        Carray& operator=(const Carray&) = delete;
        ~Carray();

    public:
        class iterator;
        void reserve(size_t capacity);
        void push(void* ptr, size_t size, const char* file, int line);
        void pop(void* ptr);
        Info* at(size_t idx);
        const Info* at(size_t idx) const;
        Info* operator[](size_t idx);
        const Info* operator[](size_t idx) const;

        // iterator 
        class iterator {
        private:
            Carray* carray;
            size_t index;

        public:
            iterator(Carray* ca, size_t idx) : carray(ca), index(idx) {}

            Info& operator*() {
                return carray->at(index)[0];
            }

            iterator& operator++() {
                if (carray && index < carray->size()) {
                    ++index;
                }
                else {
                    carray = nullptr;
                    index = -1;
                }
                return *this;
            }

            bool operator==(const iterator& other) const {
                return (carray == other.carray && index == other.index);
            }

            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }
        };

        iterator begin() { return iterator(this, 0); }
        iterator end() { return iterator(nullptr, -1); }
        iterator find(void* ptr);
    };

    class Manager {
    private:
        Manager() = default;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

        Carray carray;

    public:
        static Manager& GetInstance() {
            static Manager instance;
            return instance;
        }

        void NewCheck(void* ptr, size_t size, const char* file, int line);
        void DeleteCheck(void* ptr);
        void ReportLeaks() const;
        void CollectGarbage();
    };
} // namespace NewTracer

void* operator new(std::size_t size, const char* file, int line);
void* operator new[](std::size_t size, const char* file, int line);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, const char* file, int line);
void operator delete[](void* ptr, const char* file, int line);
