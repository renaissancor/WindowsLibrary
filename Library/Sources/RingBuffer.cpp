#include "pch.h"
#include "RingBuffer.h"

// RingBuffer.cpp

RingBuffer::RingBuffer(int capacity)
	: _capacity(capacity), _head(0), _tail(0)
{
	if (_capacity < 4) _capacity = 4;
	_buffer = new char[_capacity];
}

RingBuffer::~RingBuffer()
{
	delete[] _buffer;
}

void RingBuffer::ResizeBuffer(const int newCapacity) noexcept {
	if (newCapacity <= _capacity) return;
	char* newBuffer = new char[newCapacity]; // can throw bad_alloc 
	int usedSize = GetUsedSize();
	Peek(newBuffer, usedSize);
	delete[] _buffer;
	_buffer = newBuffer;
	_capacity = newCapacity;
	_head = 0;
	_tail = usedSize;
}

int RingBuffer::Enqueue(const char* src, int bytes) noexcept {
	if (bytes > GetFreeSize()) {
		int newCapacity = _capacity * 2; 
		while (newCapacity - GetUsedSize() - 1 < bytes) newCapacity *= 2;
		ResizeBuffer(newCapacity); 
	}
	int firstChunk = min(bytes, DirectEnqueueSize());
	memcpy_s(_buffer + _tail, _capacity - _tail, src, firstChunk);
	int remaining = bytes - firstChunk;
	if (remaining == 0) {
		_tail = (_tail + firstChunk) % _capacity;
	}
	else {
		memcpy_s(_buffer, _capacity, src + firstChunk, remaining);
		_tail = remaining;
	}
	return bytes;
}

int RingBuffer::Dequeue(char* dst, int bytes) noexcept {
	if (bytes > GetUsedSize()) bytes = GetUsedSize();
	int firstChunk = min(bytes, _capacity - _head);
	memcpy_s(dst, bytes, _buffer + _head, firstChunk);
	int remaining = bytes - firstChunk;
	if (remaining == 0) {
		_head = (_head + firstChunk) % _capacity;
	}
	else {
		memcpy_s(dst + firstChunk, bytes - firstChunk, _buffer, remaining);
		_head = remaining;
	}
	return bytes;
}

int RingBuffer::Peek(char* dst, int bytes) const noexcept {
	if (bytes > GetUsedSize()) bytes = GetUsedSize();
	int firstChunk = min(bytes, _capacity - _head);
	memcpy_s(dst, bytes, _buffer + _head, firstChunk);
	int remaining = bytes - firstChunk;
	if (remaining > 0) {
		memcpy_s(dst + firstChunk, bytes - firstChunk, _buffer, remaining);
	}
	return bytes;
}
