#include "pch.h"

#include "NewTracer.h"

void NewTracer::Manager::NewCheck(void* ptr, size_t size,
    const char* file, int line) noexcept
{
    _records.push_back({ptr, size, file, line});
    if (_verbose) printf("[ALLOC] %zu bytes at %p (File: %s, Line: %d)\n",
        size, ptr, file ? file : "(null)", line);
}

void NewTracer::Manager::DeleteCheck(void* ptr) noexcept {
    for (size_t i = 0; i < _records.size(); ++i) {
        if (_records[i].ptr == ptr) {
            printf("[FREE ] %zu bytes at %p (File: %s, Line: %d)\n",
                _records[i].size, _records[i].ptr,
                _records[i].file ? _records[i].file : "(null)",
                _records[i].line);
            _records[i] = _records.back();
            _records.pop_back();
            return;
        }
    }
}

void NewTracer::Manager::ReportLeaks() const noexcept {
    if (_records.empty()) {
        printf("No memory leaks detected.\n");
        return;
    }
    printf("==== Memory Leak Report ====\n");
    for (size_t i = 0; i < _records.size(); ++i) {
        const Info& info = _records[i];
        printf("[LEAK ] %zu bytes at %p (File: %s, Line: %d)\n",
            info.size, info.ptr,
            info.file ? info.file : "(null)",
            info.line);
    }
    printf("============================\n");
}

void NewTracer::Manager::CollectGarbage() noexcept {
    for (size_t i = 0; i < _records.size(); ++i) {
        Info& info = _records[i];
        printf("[COLLECT] %zu bytes at %p (File: %s, Line: %d)\n",
            info.size, info.ptr,
            info.file ? info.file : "(null)",
            info.line);
        free(info.ptr);
    }
    _records.clear();
}

// operator new/delete global namespace
void* operator new(std::size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc();
    NewTracer::Manager::GetInstance().NewCheck(ptr, size, file, line);
    return ptr;
}

void* operator new[](std::size_t size, const char* file, int line) noexcept {
    return operator new(size, file, line);
}

// operator delete 
void operator delete(void* ptr) noexcept {
    if (ptr) {
        NewTracer::Manager::GetInstance().DeleteCheck(ptr);
        free(ptr);
    }
}
void operator delete[](void* ptr) noexcept {
    if (ptr) {
        NewTracer::Manager::GetInstance().DeleteCheck(ptr);
        free(ptr);
    }
}

// placement delete - do nothing 
void operator delete(void* ptr, const char* file, int line) noexcept {
    (void)ptr; (void)file; (void)line;
    if (ptr) {
        // This placement delete is called 
        // if constructor throws after allocation
        NewTracer::Manager::GetInstance().DeleteCheck(ptr);
        free(ptr);
    }
}

void operator delete[](void* ptr, const char* file, int line) noexcept {
    (void)ptr; (void)file; (void)line;
    if (ptr) {
        // This placement delete is called 
        // if constructor throws after allocation
        NewTracer::Manager::GetInstance().DeleteCheck(ptr);
        free(ptr);
    }
}
