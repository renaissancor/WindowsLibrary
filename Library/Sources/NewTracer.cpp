#include "NewTracer.h"

NewTracer::Carray::Carray()
    : _data(nullptr), _size(0), _capacity(4)
{
    _data = (Info*)malloc(sizeof(Info) * _capacity);
    if (!_data) {
        throw std::bad_alloc();
    }
}

NewTracer::Carray::Carray(size_t capacity)
    : _data(nullptr), _size(0), _capacity(capacity)
{
    if (capacity < 4) _capacity = 4;
    _data = (Info*)malloc(sizeof(Info) * _capacity);
    if (!_data) {
        throw std::bad_alloc();
    }
}

NewTracer::Carray::~Carray()
{
    if (_data) free(_data);
}

void NewTracer::Carray::reserve(size_t capacity)
{
    if (capacity <= _capacity) return;
    Info* new_data = (Info*)malloc(sizeof(Info) * capacity);
    if (new_data == nullptr) {
        // malloc failed 
        throw std::bad_alloc();
        return;
    }
    if (_data) {
        for (size_t i = 0; i < _size; i++) {
            new_data[i] = _data[i];
        }
        free(_data);
    }
    _data = new_data;
    _capacity = capacity;
}

void NewTracer::Carray::push(void* ptr, size_t size, const char* file, int line)
{
    if (_size >= _capacity) {
        size_t new_capacity = _capacity > 0 ? _capacity * 2 : 4;
        reserve(new_capacity);
    }
    if (_size < _capacity) {
        _data[_size].ptr = ptr;
        _data[_size].size = size;
        _data[_size].file = file;
        _data[_size].line = line;
        _size++;
    }
}

void NewTracer::Carray::pop(void* ptr)
{
    for (size_t i = 0; i < _size; i++) {
        if (_data[i].ptr == ptr) {
            // Move the last element to the position of the removed element
            _data[i] = _data[_size - 1];
            _size--;
            return;
        }
    }
}

NewTracer::Info* NewTracer::Carray::at(size_t idx) {
    if (idx < _size) {
        return &_data[idx];
    }
    return nullptr;
}

const NewTracer::Info* NewTracer::Carray::at(size_t idx) const {
    if (idx < _size) {
        return &_data[idx];
    }
    return nullptr;
}

NewTracer::Info* NewTracer::Carray::operator[](size_t idx) {
    if (idx < _size) {
        return &_data[idx];
    }
    return nullptr;
}

const NewTracer::Info* NewTracer::Carray::operator[](size_t idx) const {
    if (idx < _size) {
        return &_data[idx];
    }
    return nullptr;
}

NewTracer::Carray::iterator NewTracer::Carray::find(void* ptr) {
    for (size_t i = 0; i < _size; ++i) {
        if (_data[i].ptr == ptr) {
            return iterator(this, i);
        }
    }
    return end();
}

void NewTracer::Manager::NewCheck(void* ptr, size_t size, const char* file, int line)
{
    carray.push(ptr, size, file, line);
    printf("[ALLOC] %zu bytes at %p (File: %s, Line: %d)\n",
        size, ptr, file ? file : "(null)", line);
}

void NewTracer::Manager::DeleteCheck(void* ptr) {
    Carray::iterator info = carray.find(ptr);
    if (info == carray.end()) {
        // fprintf(stderr, "[FREE?] pointer not tracked: %p\n", ptr);
        return; // not found 
    }
    Info& data = *info;
    printf("[FREE ] %zu bytes at %p (File: %s, Line: %d)\n",
        data.size, data.ptr,
        data.file ? data.file : "(null)",
        data.line
    );
    carray.pop(ptr);
}

void NewTracer::Manager::ReportLeaks() const {
    if (carray.empty()) {
        printf("No memory leaks detected.\n");
        return;
    }
    printf("==== Memory Leak Report ====\n");

    for (size_t i = 0; i < carray.size(); i++) {
        const Info* info = carray.at(i);
        if (info) {
            printf("[LEAK ] %zu bytes at %p (File: %s, Line: %d)\n",
                info->size, info->ptr,
                info->file ? info->file : "(null)",
                info->line);
        }
    }

    printf("============================\n");
}

void NewTracer::Manager::CollectGarbage() {
    for (size_t i = 0; i < carray.size(); i++) {
        Info* info = carray.at(i);
        if (info) {
            printf("[LEAK ] %zu bytes at %p (File: %s, Line: %d)\n",
                info->size, info->ptr,
                info->file ? info->file : "(null)",
                info->line);
            free(info->ptr);
        }
    }
}

// operator new/delete global namespace
void* operator new(std::size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc();
    NewTracer::Manager::GetInstance().NewCheck(ptr, size, file, line);
    return ptr;
}

void* operator new[](std::size_t size, const char* file, int line) {
    return operator new(size, file, line);
}

// operator delete 
void operator delete(void* ptr) {
    if (ptr) {
        NewTracer::Manager::GetInstance().DeleteCheck(ptr);
        free(ptr);
    }
}
void operator delete[](void* ptr) {
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
