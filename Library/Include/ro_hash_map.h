#pragma once 

// ro_hash_map.h 
// Input is alwaws const char* or const wchar_t* in .rodata section

namespace jhp {
	template<typename V>
	class ro_hash_map {
		struct ROHashNode {
			const char* key;
			V value;
			ROHashNode* next;
		};
		using Node = ROHashNode; 
	private:
		size_t _capacity; 
		Node** _bucket; 

		inline size_t HashFunc(const char* key) const noexcept {
			size_t sum = 0; 
			for (const char* i = key; i != nullptr; ++i) {
				sum += *(i);
			}

			size_t hash = reinterpret_cast<size_t>(key); 
			hash = (hash >> 3) ^ (hash >> 11);
			return hash % _capacity;
		}

	public:
		void insert(const char* key, V& value) noexcept {
			size_t idx = HashFunc(key);
			Node* node = _bucket[idx];
			while (node) {
				if (node->key == key) {
					node->value = value;
					return;
				}
				node = node->next;
			}
			Node* newNode = new Node{ key, value, _bucket[idx] };
			_bucket[idx] = newNode;
		}

		template<typename T>
		void insert(const char* key, T&& value) noexcept {
			size_t idx = HashFunc(key);
			Node* node = _bucket[idx];
			while (node) {
				if (node->key == key) {
					node->value = std::forward<T>(value); // move if rvalue
					return;
				}
				node = node->next;
			}
			Node* newNode = new Node{ key, std::forward<T>(value), _bucket[idx] };
			_bucket[idx] = newNode;
		}


		void erase(const char* key) noexcept {
			size_t idx = HashFunc(key);
			Node* current = _bucket[idx];
			Node* prev = nullptr;
			while (current != nullptr) {
				if (current->key == key) {
					if (prev == nullptr) {
						_bucket[idx] = current->next;
					}
					else {
						prev->next = current->next;
					}
					delete current;
					return;
				}
				prev = current;
				current = current->next;
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
		}

		V* find(const char* key) noexcept {
			size_t idx = HashFunc(key);
			Node* node = _bucket[idx];
			while (node) {
				if (node->key == key) {
					return &node->value;
				}
				node = node->next;
			}
			return nullptr;
		}

		V* at(const char* key) noexcept {
			return find(key);
		} 

		V* operator[](const char* key) noexcept {
			return find(key);
		}

		ro_hash_map(size_t capacity = 1024) noexcept 
			: _capacity(capacity) 
		{
			_bucket = new Node * [capacity](); 
		}

		~ro_hash_map() noexcept
		{
			clear(); 
		}

		class iterator {
		private:
			Node** _bucket; 
			size_t _index; 
			size_t _capacity; 
			Node*  _current;
		public:
			iterator(Node** bucket, size_t capacity, size_t index, Node* current) noexcept 
				: _bucket(bucket), _capacity(capacity), _index(index), _current(current) 
			{}
			
			iterator& operator++() noexcept {
				if (_current && _current->next) {
					_current = _current->next;
				}
				else {
					do {
						_index++;
					} while (_index < _capacity && _bucket[_index] == nullptr);
					_current = (_index < _capacity) ? _bucket[_index] : nullptr;
				}
				return *this;
			}

			std::pair<const char*, V&> operator*() noexcept {
				return { _current->key, _current->value }; 
			}

			bool operator!=(const iterator& other) const noexcept {
				return _current != other._current;
			}
		}; 

		iterator end() noexcept {
			return iterator(_bucket, _capacity, _capacity, nullptr);
		}

		iterator begin() noexcept {
			for (size_t i = 0; i < _capacity; ++i) {
				if (_bucket[i]) {
					return iterator(_bucket, _capacity, i, _bucket[i]);
				}
			}
			return end();
		}
	};
} 