#pragma once

namespace NewTracer {

    struct Info {         // 32Bit 64Bit 
        void* ptr;        // 4     8 
        size_t size;      // 4     4 
        const char* file; // 4     8 
        int line;         // 4     4 

        Info(void* p = nullptr, size_t s = 0, 
            const char* f = nullptr, int l = 0) noexcept
            : ptr(p), size(s), file(f), line(l) {
        }

        Info& operator=(const Info& other) noexcept {
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
        inline bool empty() const noexcept { return _size == 0; }
        inline size_t size() const noexcept { return _size; }
        inline size_t capacity() const noexcept { return _capacity; }

    public:
        Carray();
        Carray(size_t capacity);
        Carray(const Carray&) = delete;
        Carray& operator=(const Carray&) = delete;
        ~Carray() noexcept;

    public:
        class iterator;
        void reserve(size_t capacity);
        void push(void* ptr, size_t size, const char* file, int line) noexcept;
        void pop(void* ptr) noexcept;
        Info* at(size_t idx) noexcept;
        const Info* at(size_t idx) const noexcept;
        Info* operator[](size_t idx) noexcept;
        const Info* operator[](size_t idx) const noexcept;

        // iterator 
        class iterator {
        private:
            Carray* carray;
            size_t index;

        public:
            iterator(Carray* ca, size_t idx) noexcept 
                : carray(ca), index(idx) {}

            Info& operator*() noexcept {
                return carray->at(index)[0];
            }

            iterator& operator++() noexcept {
                if (carray && index < carray->size()) {
                    ++index;
                }
                else {
                    carray = nullptr;
                    index = INT_MAX;
                }
                return *this;
            }

            bool operator==(const iterator& other) const noexcept {
                return (carray == other.carray && index == other.index);
            }

            bool operator!=(const iterator& other) const noexcept {
                return !(*this == other);
            }
        };

        iterator begin() noexcept { return iterator(this, 0); }
        iterator end() noexcept { return iterator(nullptr, INT_MAX); }
        iterator find(void* ptr) noexcept;
    };

    class Manager {
    private:
        Manager() noexcept = default;
        ~Manager() noexcept = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

        Carray carray;

    public:
        static Manager& GetInstance() noexcept {
            static Manager instance;
            return instance;
        }

        void NewCheck(void* ptr, size_t size, const char* file, int line) noexcept;
        void DeleteCheck(void* ptr) noexcept;
        void ReportLeaks() const noexcept;
        void CollectGarbage() noexcept;
    };
} // namespace NewTracer

void* operator new(std::size_t size, const char* file, int line);
void* operator new[](std::size_t size, const char* file, int line) noexcept;
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, const char* file, int line) noexcept;
void operator delete[](void* ptr, const char* file, int line) noexcept;
