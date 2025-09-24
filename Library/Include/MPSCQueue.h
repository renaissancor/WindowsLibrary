#pragma once 

#include <wtypes.h>
#include <intrin.h>

template<typename T>
class MPSCQueue {
private:
    constexpr static const size_t CACHE_LINE_SIZE = 64;
    alignas(CACHE_LINE_SIZE) volatile size_t _head { 0 }; // Consumer Single 
	alignas(CACHE_LINE_SIZE) volatile size_t _tail { 0 }; // Producer Multiple 
    T* _buffer;
    size_t _capacity;

public:
    explicit MPSCQueue(size_t capacity)
        : _capacity(capacity)
    {
        _buffer = new T[capacity];
    }

    ~MPSCQueue() {
        delete[] _buffer;
    }

    bool Push(const T& item);
    bool Pop(T& item);
};

template<typename T>
bool MPSCQueue<T>::Push(const T& item) {
    size_t current_tail = InterlockedIncrement64((volatile LONG64*)&_tail) - 1;
    size_t next_tail = current_tail % _capacity;

    size_t current_head = _head;
    if (((current_tail + 1) % _capacity) == current_head) {
		InterlockedDecrement64((volatile LONG64*)&_tail); 
        return false; 
    }
    _buffer[next_tail] = item;
    MemoryBarrier(); 
    return true;
}

template<typename T>
bool MPSCQueue<T>::Pop(T& item) {
    size_t current_head = _head;
    size_t current_tail = _tail;

    if (current_head == current_tail) {
        return false; 
    }
    size_t next_head = (current_head + 1) % _capacity;
    MemoryBarrier(); 
    item = _buffer[current_head % _capacity];
    _head = next_head;
    return true;
}