// SPSCQueue.h 
#pragma once 

#include <wtypes.h>

template<typename T>
class SPSCQueue {
private:
	constexpr static const size_t CACHE_LINE_SIZE = 64; 
	alignas(CACHE_LINE_SIZE) volatile LONG _head{ 0 };
	alignas(CACHE_LINE_SIZE) volatile LONG _tail{ 0 };
	T* _buffer;
	size_t _capacity;

public:
	explicit SPSCQueue(size_t capacity) 
		: _capacity(capacity) {
		_buffer = new T[capacity]; 
	}

	~SPSCQueue() {
		delete[] _buffer; 
	}

	bool Push(const T& item);
	bool Pop(T& item);
	const T* Front(); 
	const T* PopFront(); 
}; 


template<typename T>
bool SPSCQueue<T>::Push(const T& item) {
	const LONG current_head = _head;
	const LONG current_tail = _tail;
	const size_t next_tail = (current_tail + 1) % _capacity;
	if (next_tail == current_head) {
		return false;
	}
	_buffer[current_tail] = item;
	_InterlockedExchange((LONG volatile*)&_tail, next_tail);
	return true;
}

template<typename T>
bool SPSCQueue<T>::Pop(T& item) {
	const LONG current_head = _head;
	const LONG current_tail = _tail;
	if (current_head == current_tail) {
		return false;
	}
	item = _buffer[current_head];
	const size_t next_head = (current_head + 1) % _capacity;
	_InterlockedExchange((LONG volatile*)&_head, next_head);
	return true;
}

template<typename T>
const T* SPSCQueue<T>::Front() {
	const LONG current_head = _head;
	const LONG current_tail = _tail; 
	if (current_head == current_tail) {
		return nullptr;
	}
	return &_buffer[current_head];
}

template<typename T>
const T* SPSCQueue<T>::PopFront() {
	const LONG current_head = _head;
	const LONG current_tail = _tail;
	if(current_head == current_tail) {
		return nullptr;
	}
	const T* item = &_buffer[current_head];
	const size_t next_head = (current_head + 1) % _capacity; 
	_InterlockedExchange((LONG volatile*)&_head, next_head);
	return item;
}
