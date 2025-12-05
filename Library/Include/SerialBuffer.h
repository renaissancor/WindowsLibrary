#pragma once 

// SerialBuffer.h 

class SerialBuffer {
	static constexpr int MAX_SEGMENT_SIZE = 1460; // Maximum Segment Size for TCP/IP
	static constexpr int BUFFER_CAPACITY = 4096; 
	static constexpr int AND_MASK = BUFFER_CAPACITY - 1; 
	// _head % BUFFER_CAPACITY == _head & AND_MASK

private:
	char _buffer[BUFFER_CAPACITY] = { 0 }; 
	int _readPos = 0;
	int _writePos = 0;

public:
	SerialBuffer() = default;
	~SerialBuffer() = default; 
	SerialBuffer(const SerialBuffer&) = delete;
	SerialBuffer& operator=(const SerialBuffer&) = delete;
	SerialBuffer(SerialBuffer&&) = delete; 
	SerialBuffer& operator=(SerialBuffer&&) = delete; 

	inline int GetCapacity() const noexcept { return BUFFER_CAPACITY; }
	inline int GetUsedSize() const noexcept { return _writePos - _readPos; }
	inline int GetFreeSize() const noexcept { return BUFFER_CAPACITY - _writePos; } 
	inline bool IsEmpty() const noexcept { return _readPos == _writePos; }

	inline const char* GetBufferPtr() noexcept { return _buffer; }
	inline const char* GetReadPtr() noexcept { return _buffer + _readPos; }
	inline const char* GetWritePtr() noexcept { return _buffer + _writePos; }

	inline int MoveReadPos(int offset) noexcept { _readPos  += offset; return offset; }
	inline int MoveWritePos(int offset) noexcept { _writePos += offset; return offset; }
	inline void Clear() noexcept { _readPos = 0; _writePos = 0; }

	template<typename T>
	inline void Put(const T& val) noexcept {
		int pos = _writePos;
		std::memcpy(_buffer + pos, &val, sizeof(T));
		_writePos = pos + sizeof(T);
	}

	template<typename T> 
	inline SerialBuffer& PutData(const T& val) noexcept {
		Put(val);
		return *this;
	}

	template<typename T>
	inline void Get(T& out) noexcept {
		int pos = _readPos;
		std::memcpy(&out, _buffer + pos, sizeof(T));
		_readPos = pos + sizeof(T);
	}

	template<typename T>
	inline SerialBuffer& GetData(T& out) noexcept {
		Get(out);
		return *this;
	}

	template<typename T>
	inline SerialBuffer& operator<<(const T& v) noexcept {
		Put(v);
		return *this;
	}

	template<typename T>
	inline SerialBuffer& operator>>(T& v) noexcept {
		Get(v);
		return *this;
	}
};
