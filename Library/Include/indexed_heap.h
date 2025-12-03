#pragma once 

// indexed_heap.h 

template<typename T>
class indexed_heap {
private:
    std::vector<int> _heap;
    std::vector<int> _pos;
    std::vector<T> _key;
    size_t _size;
private:
    void swap_node(int i, int j) noexcept {
        int id_i = _heap[i];
        int id_j = _heap[j];
        _heap[i] = id_j;
        _heap[j] = id_i;
        _pos[id_i] = j;
        _pos[id_j] = i;
    }
    void heapify_up(int index) noexcept {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (compare(index, parent)) {
                swap_node(index, parent);
                index = parent;
            }
            else {
                break;
            }
        }
    }
    void heapify_down(int index) noexcept {
        for (;;) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;
            if (left < static_cast<int>(_size) && compare(left, smallest))
                smallest = left;
            if (right < static_cast<int>(_size) && compare(right, smallest))
                smallest = right;
            if (smallest == index) break;
            swap_node(index, smallest);
            index = smallest;
        }
    }
public:
    inline bool compare(int i, int j) const noexcept
    { return _key[_heap[i]] < _key[_heap[j]]; }
    inline bool contains(int id) const noexcept
    { return id >= 0 && id < static_cast<int>(_pos.size()) && _pos[id] != -1; }
    inline bool empty() const noexcept { return _size == 0; }
    inline size_t size() const noexcept { return _size; }
    inline const T& get_priority(int id) const noexcept { return _key[id]; }

    indexed_heap(size_t capacity) noexcept
        : _heap(), _pos(), _key(), _size(0)
    {
        _heap.reserve(capacity);
        _pos.resize(capacity, -1);
        _key.resize(capacity, T());
    }
    ~indexed_heap() noexcept = default;

    void push(int id, T priority) noexcept {
        if (_pos[id] != -1) {
            decrease_key(id, priority);
            return;
        }
        int new_idx = static_cast<int>(_size);
        _key[id] = priority;
        _pos[id] = new_idx;
        _heap.push_back(id);
        ++_size;
        heapify_up(new_idx);
    }

    void decrease_key(int id, T priority) noexcept {
        if (_pos[id] == -1) return;
        if (priority >= _key[id]) return;
        _key[id] = priority;
        heapify_up(_pos[id]);
    }

    int pop() noexcept {
        if (_size == 0) return -1;
        int top_id = _heap[0];
        _pos[top_id] = -1;
        --_size;
        if (_size > 0) {
            _heap[0] = _heap[_size];
            _pos[_heap[0]] = 0;
            _heap.pop_back();
            heapify_down(0);
        }
        else {
            _heap.pop_back();
        }
        return top_id;
    }
};
