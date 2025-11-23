#pragma once

class RingBuffer {
private: 
	char* _buffer; 
	const int _capacity; 
	int _head;
	int _tail;
	int _size;

public:
	RingBuffer() = delete; 
	~RingBuffer();
	RingBuffer(int bufferCapacity);
	RingBuffer(const RingBuffer& other) = delete;
	RingBuffer& operator=(const RingBuffer& other) = delete;
public:
	inline int GetUsedSize() const { return _size; } 
	inline int GetFreeSize() const { return _capacity - _size; }

	int Enqueue(const char* data, int size);
	int Dequeue(char* data, int size);
	int Peek(char* data, int size) const; 
}; 