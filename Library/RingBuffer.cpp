#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer(int bufferCapacity)
	: _buffer(nullptr), _capacity(bufferCapacity), 
	_head(0), _tail(0), _size(0)
{
	_buffer = new char[_capacity];
}

RingBuffer::~RingBuffer(void)
{
	if (_buffer) {
		delete[] _buffer;
		_buffer = nullptr;
	}
}

int RingBuffer::Enqueue(const char* data, int size)
{
	if (size <= 0) return 0;
	if (size > GetFreeSize()) {
		size = GetFreeSize();
	}
	int firstChunkSize = min(size, _capacity - _tail);
	memcpy_s(_buffer + _tail, _capacity - _tail, data, firstChunkSize);

	_tail = (_tail + firstChunkSize) % _capacity;
	int remainingSize = size - firstChunkSize;
	if (remainingSize > 0) {
		memcpy_s(_buffer + _tail, _capacity - _tail, data + firstChunkSize, remainingSize);
		_tail = (_tail + remainingSize) % _capacity;
	}
	_size += size;
	return size;
}

int RingBuffer::Dequeue(char* data, int size) 
{
	if (size <= 0) return 0;
	if (size > GetUsedSize()) {
		size = GetUsedSize();
	}
	int firstChunkSize = min(size, _capacity - _head);
	memcpy_s(data, size, _buffer + _head, firstChunkSize);  // destination: data, source: _buffer + _head
	_head = (_head + firstChunkSize) % _capacity;
	int remainingSize = size - firstChunkSize;
	if (remainingSize > 0) {
		memcpy_s(data + firstChunkSize, remainingSize, _buffer + _head, remainingSize);
		_head = (_head + remainingSize) % _capacity;
	}
	_size -= size;
	return size;
}

int RingBuffer::Peek(char* data, int size) const 
{
	if (size <= 0) return 0;
	if (size > GetUsedSize()) {
		size = GetUsedSize();
	}
	int tempHead = _head;
	int firstChunkSize = min(size, _capacity - tempHead);
	memcpy_s(data, size, _buffer + _head, firstChunkSize);  // destination: data, source: _buffer + _head
	tempHead = (tempHead + firstChunkSize) % _capacity;
	int remainingSize = size - firstChunkSize;
	if (remainingSize > 0) {
		memcpy_s(data + firstChunkSize, remainingSize, _buffer + _head, remainingSize);
	}
	return size;
}
