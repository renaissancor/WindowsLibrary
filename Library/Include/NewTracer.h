#pragma once

#include "malloc_vector.h"

namespace NewTracer {

    struct Info {         // 32Bit 64Bit 
        void* ptr;        // 4     8 
        size_t size;      // 4     8 
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

        inline bool operator==(const Info& other) const noexcept {
            return ptr == other.ptr;
		}
    };

    class Manager {
    private:
        Manager() noexcept = default;
        ~Manager() noexcept = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

        bool _verbose = true;
        malloc_vector<Info> _records;

    public:
        inline void SetVerbose(bool v) noexcept { _verbose = v; }

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
