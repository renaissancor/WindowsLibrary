#pragma once
// darray.h 
// Dynamic Array 

namespace jhp {
	template<typename T>
	class darray {
	private:
		T* _data;
		size_t _capacity;
		size_t _size;
	public:
		inline bool empty() const noexcept { return _size == 0; }
		inline size_t size() const noexcept { return _size; }
		inline size_t capacity() const noexcept { return _capacity; }
		inline void clear() noexcept { _size = 0; }
	public: // Constructors & Destructor 
		darray() noexcept;
		darray(size_t capacity) noexcept;
		darray(const darray&) = delete;
		darray& operator=(const darray&) = delete;
		~darray() noexcept;
	public: // Modifiers 
		void reserve(size_t capacity) noexcept;
		void resize(size_t size) noexcept;
		void push_back(const T& item) noexcept;
		void pop_back() noexcept;
	public: // Element Access 
		T& at(size_t idx) noexcept;
		const T& at(size_t idx) const noexcept;
		T& operator[](size_t idx) noexcept;
		const T& operator[](size_t idx) const noexcept;
	public: // Iterators 
		class iterator;
		iterator begin() noexcept;
		iterator end() noexcept;
		iterator erase(iterator& target) noexcept;
	};

	template<typename T>
	darray<T>::darray() 
		: _data(nullptr), _capacity(0), _size(0) 
	{}

	template<typename T>
	darray<T>::darray(size_t capacity) 
		: _data(new T[capacity]), _capacity(capacity), _size(0) 
	{} 

	template<typename T> 
	darray<T>::~darray() noexcept {
		if (_data) delete[] _data;
	}

	template<typename T>
	void darray<T>::reserve(size_t capacity) noexcept {
		if (capacity <= _capacity) return; 
		T* new_data = new T[capacity];
		for (size_t i = 0; i < _size; i++) {
			new_data[i] = _data[i];
		}
		if (_data) delete[] _data;
		_data = new_data;
		_capacity = capacity;
	}

	template<typename T>
	void darray<T>::resize(size_t size) noexcept {
		if (size > _capacity) {
			reserve(size);
		}
		if (size > _size) {
			for (size_t i = _size; i < size; i++) {
				_data[i] = T();
			}
		}
		_size = size;
	}

	template<typename T>
	void darray<T>::push_back(const T& item) noexcept {
		if (_capacity <= _size) {
			reserve(_capacity == 0 ? 1 : _capacity * 2);
		}
		_data[_size++] = item;
	}

	template<typename T>
	void darray<T>::pop_back() noexcept {
		--_size;
	}

	template<typename T>
	T& darray<T>::at(size_t idx) noexcept {
		return _data[idx];
	}

	template<typename T>
	const T& darray<T>::at(size_t idx) const noexcept {
		return _data[idx];
	}

	template<typename T>
	T& darray<T>::operator[](size_t idx) noexcept {
		return _data[idx];
	}

	template<typename T>
	const T& darray<T>::operator[](size_t idx) const noexcept {
		return _data[idx];
	}

	template<typename T>
	class darray<T>::iterator {
	private:
		T* _ptr;
	public:
		iterator(T* ptr) noexcept : _ptr(ptr) {}
		iterator& operator++() noexcept { ++_ptr; return *this; }
		iterator operator++(int) noexcept { iterator temp = *this; ++_ptr; return temp; }
		T& operator*() noexcept { return *_ptr; }
		bool operator!=(const iterator& other) const noexcept { return _ptr != other._ptr; }
		bool operator==(const iterator& other) const noexcept { return _ptr == other._ptr; }
		T* operator->() noexcept { return _ptr; }
		friend class darray;
	};

	template<typename T>
	typename darray<T>::iterator darray<T>::begin() noexcept {
		return iterator(_data);
	}

	template<typename T>
	typename darray<T>::iterator darray<T>::end() noexcept {
		return iterator(_data + _size);
	}

	template<typename T>
	typename darray<T>::iterator darray<T>::erase(iterator& target) noexcept {
		if (target._ptr < _data || target._ptr >= _data + _size) {
			return end();
		}
		size_t idx = target._ptr - _data;
		for (size_t i = idx; i < _size - 1; i++) {
			_data[i] = _data[i + 1];
		}
		--_size;
		return iterator(_data + idx);
	}

}