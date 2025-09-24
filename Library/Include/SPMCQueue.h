#pragma once 

#include <wtypes.h>
#include <intrin.h>

template<typename T>
class SPMCQueue {
private:
	alignas(64) volatile size_t _head { 0 }; // Consumer Multiple
	alignas(64) volatile size_t _tail { 0 }; // Producer Single
    T* _buffer;
    size_t _capacity;

public:
    explicit SPMCQueue(size_t capacity)
        : _capacity(capacity) {
        _buffer = new T[capacity];
    }

    ~SPMCQueue() {
        delete[] _buffer;
    }

    bool Push(const T& item);
    bool Pop(T& item); // Several Consumers Pop 
};

template<typename T>
bool SPMCQueue<T>::Push(const T& item) {
    size_t current_tail = _tail;
    size_t next_tail = (current_tail + 1) % _capacity;

    size_t current_head = _head;
    if (next_tail == current_head) {
        return false; 
    }
	MemoryBarrier(); 
    _buffer[current_tail] = item;
	MemoryBarrier(); 
	_tail = next_tail; // Producer Single No Atomics Needed
    return true;
}

template<typename T>
bool SPMCQueue<T>::Pop(T& item) {
    size_t current_tail = _tail;
    size_t current_head = InterlockedIncrement64((volatile LONG64*)&_head) - 1;

    if (current_head == current_tail) {
        return false; 
    }

    item = _buffer[current_head % _capacity];
    return true;
}
