#pragma once 

// cstr_hash_map: A hash map implementation with C-style string keys and generic value type V.

template<typename V>
class cstr_hash_map {
private:
	static constexpr float LOAD_FACTOR = 0.75f;

	struct Node {
		const char* key;
		V value;
		Node* next;
		Node(const char* k, V v) noexcept : key(k), value(v), next(nullptr) {}
	};

	size_t _capacity;
	size_t _size;
	Node** _bucket;

private: 

	inline static int cstr_cmp(const char* a, const char* b) noexcept {
		if (a == b) return 0; 
		while (*a && (*a == *b)) { ++a; ++b; }
		return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
	}

	inline static size_t hash_func(const char* key) noexcept {
		size_t hash = 5381;
		while (*key) {
			hash = ((hash << 5) + hash) + static_cast<unsigned char>(*key++);
		}
		return hash;
	}

	void rehash() noexcept { 
		size_t newCapacity = _capacity * 2;
		Node** newBucket = new Node * [newCapacity](); // can throw std::bad_alloc but ignore 
		for (size_t i = 0; i < _capacity; ++i) {
			Node* node = _bucket[i];
			while (node) {
				Node* nextNode = node->next;
				size_t idx = hash_func(node->key) % newCapacity;
				node->next = newBucket[idx];
				newBucket[idx] = node;
				node = nextNode;
			}
		}
		delete[] _bucket;
		_bucket = newBucket;
		_capacity = newCapacity;
	}

public:
	cstr_hash_map(const cstr_hash_map&) = delete;
	cstr_hash_map& operator=(const cstr_hash_map&) = delete;
	cstr_hash_map(cstr_hash_map&&) = delete;
	cstr_hash_map& operator=(cstr_hash_map&&) = delete;

	size_t size() const noexcept { return _size; }
	bool empty() const noexcept { return _size == 0; }
	bool contains(const char* key) noexcept { return find(key) != end(); }
	float load_factor() const noexcept {
		return static_cast<float>(_size) / static_cast<float>(_capacity);
	}
	void reserve(size_t new_capacity) noexcept {
		while (_capacity < new_capacity) rehash();
	}

	class iterator {
	private:
		Node* _node;
		Node** _buckets;
		size_t _capacity;
		size_t _bucket_index;

	public:
		iterator() noexcept
			: _node(nullptr), _buckets(nullptr), _capacity(0), _bucket_index(0) {
		}

		iterator(Node* n, Node** b, size_t cap, size_t idx) noexcept
			: _node(n), _buckets(b), _capacity(cap), _bucket_index(idx) {
		}

		const char* key() const noexcept { return _node->key; }
		V& value() noexcept { return _node->value; }
		const V& value() const noexcept { return _node->value; }

		std::pair<const char*, V&> operator*() noexcept {
			return { _node->key, _node->value };
		}

		V* operator->() noexcept { return &(_node->value); }

		bool operator==(const iterator& other) const noexcept {
			return _node == other._node;
		}
		bool operator!=(const iterator& other) const noexcept {
			return _node != other._node;
		}

		iterator& operator++() noexcept {
			if (_node && _node->next) {
				_node = _node->next;
				return *this;
			}
			for (++_bucket_index; _bucket_index < _capacity; ++_bucket_index) {
				if (_buckets[_bucket_index]) {
					_node = _buckets[_bucket_index];
					return *this;
				}
			}
			_node = nullptr;
			_bucket_index = _capacity;
			return *this;
		}

		iterator operator++(int) noexcept { 
			iterator temp = *this;
			++(*this);
			return temp;
		}
	};

	class const_iterator {
	private:
		const Node* _node;
		Node* const* _buckets;
		size_t _capacity;
		size_t _bucket_index;
	public:
		const_iterator() noexcept
			: _node(nullptr), _buckets(nullptr), _capacity(0), _bucket_index(0) {
		}

		const_iterator(const Node* n, Node* const* b, size_t cap, size_t idx) noexcept
			: _node(n), _buckets(b), _capacity(cap), _bucket_index(idx) {
		}

		const char* key() const noexcept { return _node->key; }
		const V& value() const noexcept { return _node->value; }

		std::pair<const char*, const V&> operator*() const noexcept {
			return { _node->key, _node->value };
		}

		const_iterator& operator++() noexcept {
			if (_node && _node->next) {
				_node = _node->next;
				return *this;
			}
			for (++_bucket_index; _bucket_index < _capacity; ++_bucket_index) {
				if (_buckets[_bucket_index]) {
					_node = _buckets[_bucket_index];
					return *this;
				}
			}
			_node = nullptr;
			_bucket_index = _capacity;
			return *this;
		}

		bool operator==(const const_iterator& other) const noexcept {
			return _node == other._node;
		}
		bool operator!=(const const_iterator& other) const noexcept {
			return _node != other._node;
		}
	};

	const_iterator begin() const noexcept {
		for (size_t i = 0; i < _capacity; ++i) {
			if (_bucket[i]) {
				return const_iterator(_bucket[i], _bucket, _capacity, i);
			}
		}
		return end();
	}

	const_iterator end() const noexcept {
		return const_iterator(nullptr, _bucket, _capacity, _capacity);
	}


	iterator begin() noexcept {
		for (size_t i = 0; i < _capacity; ++i) {
			if (_bucket[i]) {
				return iterator(_bucket[i], _bucket, _capacity, i);
			}
		}
		return end();
	}

	iterator end() noexcept {
		return iterator(nullptr, _bucket, _capacity, _capacity);
	}

	iterator find(const char* key) noexcept {
		size_t idx = hash_func(key) % _capacity;
		Node* node = _bucket[idx];
		while (node) {
			if (cstr_cmp(node->key, key) == 0) {
				return iterator(node, _bucket, _capacity, idx);
			}
			node = node->next;
		}
		return end();
	}

	void insert(const char* key, V value) noexcept {
		if (_size >= static_cast<size_t>(_capacity * LOAD_FACTOR)) {
			rehash();
		}
		size_t idx = hash_func(key) % _capacity;
		Node* node = _bucket[idx];
		while (node) {
			if (cstr_cmp(node->key, key) == 0) {
				node->value = value;
				return;
			}
			node = node->next;
		}
		Node* newNode = new Node(key, value); // can throw std::bad_alloc but ignore 
		newNode->next = _bucket[idx];
		_bucket[idx] = newNode;
		++_size;
	}

	V& operator[](const char* key) noexcept {
		const size_t hash_value = hash_func(key); 
		size_t idx = hash_value % _capacity;
		Node* node = _bucket[idx];

		while (node) {
			if (cstr_cmp(node->key, key) == 0) {
				return node->value;
			}
			node = node->next;
		}

		if (_size >= static_cast<size_t>(_capacity * LOAD_FACTOR)) {
			rehash();
			idx = hash_value % _capacity;
		}

		Node* newNode = new Node(key, V{});
		newNode->next = _bucket[idx];
		_bucket[idx] = newNode;
		++_size;
		return newNode->value;
	}

	void erase(const char* key) noexcept {
		size_t idx = hash_func(key) % _capacity;
		Node* curr = _bucket[idx];
		Node* prev = nullptr;
		while (curr) {
			if (cstr_cmp(curr->key, key) == 0) {
				if (prev == nullptr) {
					_bucket[idx] = curr->next;
				}
				else {
					prev->next = curr->next;
				}
				delete curr;
				--_size;
				return;
			}
			prev = curr;
			curr = curr->next;
		}
	}

	void clear() noexcept {
		for (size_t i = 0; i < _capacity; ++i) {
			Node* node = _bucket[i];
			while (node) {
				Node* temp = node;
				node = node->next;
				delete temp;
			}
			_bucket[i] = nullptr;
		}
		_size = 0;
	}

	explicit cstr_hash_map(size_t capacity = 64)
		: _capacity(capacity), _size(0), _bucket(nullptr)
	{
		_bucket = new Node * [_capacity]();
	}

	~cstr_hash_map() noexcept {
		clear();
		delete[] _bucket;
	}
};

/*
Intended to use ONLY C-style string keys (const char*) generated in .rodata segment. 
Implementation of a hash map with C-style string keys and generic value type V.  
Does not consider std::bad_alloc and other exceptions for simplicity. 
Goal is, using only C-style string written raw, and saved at .rodata segment, 
to have a simple hash map with good performance. 
Features:
1. cstr_hash_map Class Template:
   - A hash map implementation that uses C-style strings (const char*) as keys and a generic type V as values.
   - Provides methods for insertion, deletion, lookup, and iteration over key-value pairs.
2. Iterator Class:
   - Nested class within cstr_hash_map to facilitate iteration over the map's elements.
   - Supports standard iterator operations like incrementing and dereferencing.
3. Hash Function:
   - A static method that computes a hash value for a given C-style string using the djb2 hash algorithm.
   - Rehashing mechanism to maintain performance as the number of elements grows. 
   - Used FNV-1a hash algorithm first, but switched to djb2 for better speed 
4. Collision Handling:
   - Uses separate chaining (linked lists) to handle hash collisions.
   - Each bucket in the hash table points to a linked list of nodes that share the same hash index.
5. Load Factor Management:
   - Automatically resizes the hash table when the load factor exceeds a predefined threshold (0.75).
   - Ensures efficient performance for insertions and lookups.
*/
