#pragma once

// RingBuffer.h

class RingBuffer {
protected: 
	char* _buffer; 
	int _capacity; 
	int _head;
	int _tail;

public:
	RingBuffer() = delete; 
	~RingBuffer();
	RingBuffer(int bufferCapacity);
	RingBuffer(const RingBuffer& other) = delete;
	RingBuffer& operator=(const RingBuffer& other) = delete;

protected:
	void ResizeBuffer(const int newCapacity) noexcept;

public:
	inline bool IsEmpty() const noexcept { return _head == _tail; }
	inline bool IsFull() const noexcept { return ((_tail + 1) % _capacity) == _head; }

	inline int GetHeadIndex() const noexcept { return _head; }
	inline int GetTailIndex() const noexcept { return _tail; }
	inline int GetCapacity() const noexcept { return _capacity; }

	inline int GetUsedSize() const noexcept { 
		if (_tail >= _head) return _tail - _head;
		else return _capacity - _head + _tail; 
	}
	inline int GetFreeSize() const noexcept { return _capacity - GetUsedSize() - 1; }

	inline int MoveHead(int offset) noexcept {
		_head = (_head + offset) % _capacity;
		return offset;
	}
	inline int MoveTail(int offset) noexcept {
		_tail = (_tail + offset) % _capacity;
		return offset;
	}

	inline int DirectEnqueueSize() const noexcept
	{
		if (IsFull()) return 0;
		if (_tail >= _head) return (_capacity - _tail) - ((_head == 0) ? 1 : 0);
		else return _head - _tail - 1;
	}

	inline int DirectDequeueSize() const noexcept
	{
		return (_tail < _head) ? _capacity - _head : _tail - _head;
	}

	int Enqueue(const char* src, int size) noexcept;
	int Dequeue(char* dst, int size) noexcept;
	int Peek(char* dst, int size) const noexcept;
}; 
