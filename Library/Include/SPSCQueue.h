// SPSCQueue.h 
#pragma once 

using std::atomic; 

template<typename T>
class SPSCQueue {
private:
	constexpr static const size_t CACHE_LINE_SIZE = 64; 
	alignas(CACHE_LINE_SIZE) atomic<size_t> _head{ 0 };
	alignas(CACHE_LINE_SIZE) atomic<size_t> _tail{ 0 };
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
	const size_t current_tail = _tail.load(std::memory_order_relaxed);
	const size_t current_head = _head.load(std::memory_order_relaxed);

	const size_t next_tail = (current_tail + 1) % _capacity;
	if (next_tail == current_head) {
		// Queue is full 
		return false;
	}

	_buffer[current_tail] = item;
	_tail.store(next_tail, std::memory_order_release);
	return true;
}

template<typename T>
bool SPSCQueue<T>::Pop(T& item) {
	const size_t current_head = _head.load(std::memory_order_relaxed);
	const size_t current_tail = _tail.load(std::memory_order_relaxed);
	if (current_head == current_tail) {
		return false;
	}
	item = _buffer[current_head];
	const size_t next_head = (current_head + 1) % _capacity;
	_head.store(next_head, std::memory_order_release);
	return true;
}

template<typename T>
const T* SPSCQueue<T>::Front() {
	const size_t current_head = _head.load(std::memory_order_relaxed);
	const size_t current_tail = _tail.load(std::memory_order_acquire);
	if (current_head == current_tail) {
		return nullptr;
	}
	return &_buffer[current_head];
}

template<typename T>
const T* SPSCQueue<T>::PopFront() {
	const size_t current_head = _head.load(std::memory_order_relaxed);
	const size_t current_tail = _tail.load(std::memory_order_acquire);
	if (current_head == current_tail) {
		return nullptr;
	}
	const T* item = &_buffer[current_head];
	const size_t next_head = (current_head + 1) % _capacity;
	_head.store(next_head, std::memory_order_release);
	return item;
}
