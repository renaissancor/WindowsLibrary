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

	inline void PutBytes(const void* src, size_t n) noexcept {
		memcpy(_buffer + _writePos, src, n);
		_writePos += (int)n;
	}
	inline void GetBytes(void* dst, size_t n) noexcept {
		memcpy(dst, _buffer + _readPos, n);
		_readPos += (int)n;
	}

	template<typename T, size_t Size = sizeof(T)>
	struct SerialBufferHelper {
		static inline void Put(char* buffer, int& writePos, const T& val) noexcept {
			memcpy(buffer + writePos, &val, Size);
			writePos += Size;
		}
		static inline void Get(const char* buffer, int& readPos, T& out) noexcept {
			memcpy(&out, buffer + readPos, Size);
			readPos += Size;
		}
	};

	template<typename T>
	struct SerialBufferHelper<T, 1> {
		static void Put(char* buffer, int& writePos, const T& val) noexcept {
			buffer[writePos++] = *reinterpret_cast<const unsigned char*>(&val);
		}
		static void Get(const char* buffer, int& readPos, T& out) noexcept {
			out = *reinterpret_cast<const unsigned char*>(buffer + readPos++);
		}
	};

	template<typename T>
	SerialBuffer& PutData(const T& val) noexcept {
		SerialBufferHelper<T>::Put(_buffer, _writePos, val);
		return *this;
	}

	template<typename T>
	SerialBuffer& GetData(T& out) noexcept {
		SerialBufferHelper<T>::Get(_buffer, _readPos, out);
		return *this;
	}

	inline SerialBuffer& operator<<(char value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(unsigned char value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(short value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(unsigned short value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(int value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(unsigned int value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(long value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(unsigned long value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(long long value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(unsigned long long value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(float value) noexcept { return PutData(value); }
	inline SerialBuffer& operator<<(double value) noexcept { return PutData(value); }

	inline SerialBuffer& operator>>(char& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(unsigned char& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(short& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(unsigned short& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(int& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(unsigned int& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(long& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(unsigned long& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(long long& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(unsigned long long& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(float& value) noexcept { return GetData(value); }
	inline SerialBuffer& operator>>(double& value) noexcept { return GetData(value); }
};
