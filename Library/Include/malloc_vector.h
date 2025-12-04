#pragma once

// malloc_vector.h 

template<typename T>
class malloc_vector {
private:
	T* _data;
	size_t _size;
	size_t _capacity;

public:
	malloc_vector(size_t capacity = 4);
	~malloc_vector() noexcept;
	malloc_vector(const malloc_vector&) = delete;
	malloc_vector& operator=(const malloc_vector&) = delete;

	class iterator {
	private:
		T* _ptr;
	public:
		inline iterator(T* p) noexcept : _ptr(p) {}
		inline T& operator*() noexcept { return *_ptr; }
		inline T* operator->() noexcept { return _ptr; }
		inline iterator& operator++() noexcept { ++_ptr; return *this; }
		inline iterator operator++(int) noexcept { iterator temp = *this; ++(*this); return temp; }
		inline iterator& operator--() noexcept { --_ptr; return *this; }
		inline iterator operator--(int) noexcept { iterator temp = *this; --(*this); return temp; }
		inline bool operator==(const iterator& other) const noexcept { return _ptr == other._ptr; }
		inline bool operator!=(const iterator& other) const noexcept { return _ptr != other._ptr; }
	};

	class const_iterator {
	private:
		const T* _ptr;
	public:
		inline const_iterator(const T* p) noexcept : _ptr(p) {}
		inline const T& operator*() const noexcept { return *_ptr; }
		inline const T* operator->() const noexcept { return _ptr; }
		inline const_iterator& operator++() noexcept { ++_ptr; return *this; }
		inline const_iterator operator++(int) noexcept { const_iterator temp = *this; ++(*this); return temp; }
		inline const_iterator& operator--() noexcept { --_ptr; return *this; }
		inline const_iterator operator--(int) noexcept { const_iterator temp = *this; --(*this); return temp; }
		inline bool operator==(const const_iterator& other) const noexcept { return _ptr == other._ptr; }
		inline bool operator!=(const const_iterator& other) const noexcept { return _ptr != other._ptr; }
	}; 

	inline iterator begin() noexcept { return iterator(_data); }
	inline iterator end() noexcept { return iterator(_data + _size); }
	inline const_iterator begin() const noexcept { return const_iterator(_data); }
	inline const_iterator end() const noexcept { return const_iterator(_data + _size); }

	iterator find(const T& value) noexcept; 
	iterator erase(iterator pos) noexcept; 
	iterator insert(iterator pos, const T& value) noexcept; 

	inline bool empty() const noexcept { return _size == 0; }
	inline size_t size() const noexcept { return _size; }
	inline size_t capacity() const noexcept { return _capacity; }

	void reserve(size_t capacity) noexcept; 
	void resize(size_t size) noexcept; 
	void push_back(const T& value) noexcept;
	// void emplace_back(const T& value) noexcept; // can not use, as new operator should NOT be used 

	inline void pop_back() noexcept { if (_size > 0) { --_size; } }
	inline void clear() noexcept { _size = 0; }

	inline T& at(size_t idx) noexcept { return _data[idx]; }
	inline const T& at(size_t idx) const noexcept { return _data[idx]; }
	inline T& operator[](size_t idx) noexcept { return _data[idx]; }
	inline const T& operator[](size_t idx) const noexcept { return _data[idx]; }
	inline T& front() noexcept { return _data[0]; }
	inline const T& front() const noexcept { return _data[0]; }
	inline T& back() noexcept { return _data[_size - 1]; }
	inline const T& back() const noexcept { return _data[_size - 1]; }
	inline T* data() noexcept { return _data; }
	inline const T* data() const noexcept { return _data; }
}; 

template<typename T>
malloc_vector<T>::malloc_vector(size_t capacity)
	: _data(nullptr), _size(0), _capacity(0) {
	if (capacity > 0) {
		_data = static_cast<T*>(malloc(sizeof(T) * capacity));
		if (_data) _capacity = capacity;
	}
}

template<typename T>
malloc_vector<T>::~malloc_vector() noexcept {
	if (_data) free(_data);
}

template<typename T>
void malloc_vector<T>::reserve(size_t capacity) noexcept {
	if (capacity <= _capacity) return;
	T* new_data = static_cast<T*>(malloc(sizeof(T) * capacity));
	if (new_data == nullptr) return;
	if (_data) {
		memcpy(new_data, _data, sizeof(T) * _size);
		free(_data);
	}
	_data = new_data;
	_capacity = capacity;
}

template<typename T>
void malloc_vector<T>::resize(size_t size) noexcept {
	if (size > _capacity) {
		reserve(size);
	}
	if (size <= _capacity) {
		_size = size;
	}
}

template<typename T>
void malloc_vector<T>::push_back(const T& value) noexcept {
	if (_size >= _capacity) {
		size_t new_capacity = _capacity > 0 ? _capacity * 2 : 4;
		reserve(new_capacity);
	}
	if (_size < _capacity) {
		_data[_size] = value;
		_size++;
	}
}

template<typename T>
typename malloc_vector<T>::iterator malloc_vector<T>::find(const T& value) noexcept {
	for (size_t i = 0; i < _size; ++i) {
		if (_data[i] == value) {
			return iterator(&_data[i]);
		}
	}
	return end();
}

template<typename T>
typename malloc_vector<T>::iterator malloc_vector<T>::erase(iterator pos) noexcept {
	T* ptr = pos.operator->();
	size_t index = static_cast<size_t>(ptr - _data);
	if (index >= _size) return end();
	for (size_t i = index; i < _size - 1; ++i) {
		_data[i] = _data[i + 1];
	}
	--_size;
	return iterator(&_data[index]);
}

template<typename T>
typename malloc_vector<T>::iterator malloc_vector<T>::insert(iterator pos, const T& value) noexcept {
	T* ptr = pos.operator->();
	size_t index = static_cast<size_t>(ptr - _data);
	if (index > _size) return end();
	if (_size >= _capacity) {
		size_t new_capacity = _capacity > 0 ? _capacity * 2 : 4;
		reserve(new_capacity);
	}
	for (size_t i = _size; i > index; --i) {
		_data[i] = _data[i - 1];
	}
	_data[index] = value;
	++_size;
	return iterator(&_data[index]);
}

/*
STL std::vector like dynamic array implementation using malloc/free internally. 
Goal : operator new / delete replacement for vector-like dynamic array without using new/delete internally. 
new operator overloads should use malloc_vector to track allocations and avoid infinite recursive new call. 
*/
