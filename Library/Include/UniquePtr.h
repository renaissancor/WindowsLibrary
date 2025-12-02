#pragma once

// UniquePtr.h 

template<typename T>
class UniquePtr {
private:
	T* _ptr;
public:
	explicit UniquePtr(T* p = nullptr) : _ptr(p) {}
	~UniquePtr() noexcept { delete _ptr; }
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;

	UniquePtr(UniquePtr&& other) noexcept : _ptr(other._ptr) {
		other._ptr = nullptr;
	}
	UniquePtr& operator=(UniquePtr&& other) noexcept {
		if (this == &other) return *this;
		Swap(other);
		other.Reset();
		return *this;
	}

	inline T* Get() const noexcept { return _ptr; }
	T& operator*() const noexcept { return *_ptr; }
	T* operator->() const noexcept { return _ptr; }

	explicit operator bool() const noexcept { return _ptr != nullptr; } 

	inline void Swap(UniquePtr& other) noexcept {
		T* temp = _ptr;
		_ptr = other._ptr;
		other._ptr = temp;
	}

	inline T* Release() noexcept {
		T* temp = _ptr;
		_ptr = nullptr;
		return temp;
	}
	inline void Reset(T* p = nullptr) noexcept {
		if (_ptr != p) {
			delete _ptr;
			_ptr = p;
		}
	}

};

template<typename T>
class UniquePtr<T[]> {
private:
	T* _ptr;

public:
	explicit UniquePtr(T* p = nullptr) : _ptr(p) {}
	~UniquePtr() noexcept { delete[] _ptr; }
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;

	UniquePtr(UniquePtr&& other) noexcept : _ptr(other._ptr) {
		other._ptr = nullptr;
	}
	UniquePtr& operator=(UniquePtr&& other) noexcept {
		if (this == &other) return *this;
		Swap(other);
		other.Reset();
		return *this;
	}

	inline T* Get() const noexcept { return _ptr; }
	inline  T& operator[](std::size_t index) const noexcept { return _ptr[index]; }
	explicit inline operator bool() const noexcept { return _ptr != nullptr; }
	inline void Swap(UniquePtr& other) noexcept {
		T* temp = _ptr;
		_ptr = other._ptr;
		other._ptr = temp;
	}
	inline T* Release() noexcept {
		T* temp = _ptr;
		_ptr = nullptr;
		return temp;
	}
	inline void Reset(T* p = nullptr) noexcept {
		if (_ptr != p) {
			delete[] _ptr;
			_ptr = p;
		}
	}
};

template<typename T>
inline void Swap(UniquePtr<T>& a, UniquePtr<T>& b) noexcept {
	a.Swap(b);
}

template<typename T>
inline void Swap(UniquePtr<T[]>& a, UniquePtr<T[]>& b) noexcept {
	a.Swap(b);
}

template<typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args&&... args) {
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
inline UniquePtr<T[]> MakeUniqueArray(std::size_t size) {
	return UniquePtr<T[]>(new T[size]());
}