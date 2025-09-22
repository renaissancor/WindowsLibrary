#include <assert.h>

// PQHeap is a Binary Tree with specific ordering (min or max) 
// properties, typically implemented using a dynamic array.
// C++ STL provides heap-based algorithms and containers:
//   - std::make_heap, std::push_heap, std::pop_heap
//   - std::priority_queue is a heap-based container.
// Note: OS heap (dynamic memory) is unrelated to this data structure.

template<typename T>
class PQHeap {
private:
	T*	 m_Data;
	int	 m_Capacity;
	int	 m_Size;

protected: 

	virtual bool compare(const T& a, const T& b) const = 0; 

	int getParent(int i)
	{
		return (i - 1) / 2; 
	}

	int getLeftChild(int i)
	{
		return 2 * i + 1; 
	}

	int getRightChild(int i)
	{
		return 2 * i + 2; 
	}

public:
	const T& top() const
	{
		assert(m_Size > 0);
		return *m_Data; 
	}

	bool empty() const
	{
		return m_Size == 0;
	}

	int size() const
	{
		return m_Size; 
	}

	class iterator; // Forward declaration 
	
	iterator begin()
	{
		if (m_Size == 0)
			return iterator(this, -1);
		else
			return iterator(this, 0);
	}

	iterator end()
	{
		return iterator(this, -1);
	}

public: // function declaration 
	PQHeap(); 
	~PQHeap();

	class iterator
	{
	private:
		PQHeap<T>* m_Owner;
		int		   m_Idx;

	public: 
		T& operator*()
		{
			return m_Owner->m_Data[m_Idx];
		}

		iterator& operator ++()
		{
			assert(m_Idx != -1);
			++m_Idx;
			if (m_Owner->m_Size <= m_Idx)
			{
				m_Idx = -1;
			}
			return *this;
		}
		
		iterator operator ++(int _Data)
		{
			iterator copyiter(*this);
			++(*this);
			return copyiter;
		}

		bool operator == (const iterator& _Other)
		{
			return (m_Owner == _Other.m_Owner && m_Idx == _Other.m_Idx);
		}

		bool operator !=(const iterator& _Other)
		{
			return !((*this) == _Other);
		}
	
		iterator()
			: m_Owner(nullptr)
			, m_Idx(-1) {
		}

		iterator(PQHeap<T>* owner, int idx)
			: m_Owner(owner), m_Idx(idx) {
		}

		~iterator() {
		}

		friend class PQHeap; 
	};


public: 
	void reserve(int _Capacity); 
	void resize(int _Size); 

	void swap(int i, int j);
	void heapifyUp(int index);
	void heapifyDown(int idx); 

	void push(const T& _Data);
	T pop();

	void sortHelper(int idx, int heapSize); // Helper function for sort()
	void sort(); // PQHeap Sort of Dynamic Array 
};


template<typename T>
PQHeap<T>::PQHeap()
	: m_Data(nullptr)
	, m_Capacity(0)
	, m_Size(0)
{
}

template<typename T>
PQHeap<T>::~PQHeap()
{
	delete[] m_Data;
}

template<typename T>
void PQHeap<T>::reserve(int _Capacity)
{
	if (0 == _Capacity)
	{ 
		_Capacity = 1;
	}
		

	T* pNewData = new T[_Capacity]; // Allocate New Space 

	for (int i = 0; i < m_Size; ++i)
	{
		pNewData[i] = m_Data[i]; // Copy Data 
	}

	delete[] m_Data;

	m_Data = pNewData;

	m_Capacity = _Capacity;
}


template<typename T>
void PQHeap<T>::resize(int _Size)
{
	if (m_Size < _Size)
	{
		reserve(_Size); 
	}

	for (int i = 0; i < _Size - m_Size; ++i)
	{
		m_Data[m_Size + i] = T(); // Default Generator 
	}

	m_Size = _Size; 
}

template<typename T>
void PQHeap<T>::swap(int i, int j)
{
	T temp = *(m_Data + i); 
	*(m_Data + i) = *(m_Data + j); 
	*(m_Data + j) = temp; 
}

template<typename T>
void PQHeap<T>::heapifyUp(int index)
{
	// Used for push()
	// when new element is added to the end of the dynamic array 
	// Swap it until it reaches to the root 

	while (index > 0)
	{
		int parent = getParent(index); 
		if (compare(m_Data[index], m_Data[parent]))
		{
			swap(index, parent);
			index = parent; 
		}
		else {
			break; 
		}
	}

	// After push, from end to root maximum O(log N) comparison happens 
	// Result will make sure every parents have priority than children 	
}


template<typename T>
void PQHeap<T>::push(const T& _Data)
{
	if (m_Size == m_Capacity) {
		reserve(m_Capacity == 0 ? 1 : m_Capacity * 2);
	}

	m_Data[m_Size] = _Data;
	heapifyUp(m_Size);
	++m_Size;
}

template<typename T>
void PQHeap<T>::heapifyDown(int idx)
{
	// Used for pop
	// Find the min or max by searching all child nodes 
	// Pivot is considered as the root node of the heapify down, ignoring formal array index 
	while (true)
	{
		int left = getLeftChild(idx); 
		int right = getRightChild(idx); 
		int selected = idx; 

		if (left < m_Size && compare(m_Data[left], m_Data[selected]))
			selected = left;
		if (right < m_Size && compare(m_Data[right], m_Data[selected]))
			selected = right;

		if (selected == idx)
		{
			break; 
		}
		else
		{
			swap(idx, selected); 
			idx = selected; 
		}
	}
	// After pop, the root value should be empty. 
}



template<typename T>
T PQHeap<T>::pop()
{
	assert(!(this->empty()));

	T root = m_Data[0]; 
	m_Data[0] = m_Data[--m_Size]; 

	if (!this->empty()) {
		heapifyDown(0);
	}

	return root; 
}



template<typename T>
void PQHeap<T>::sortHelper(int idx, int heapSize)
{
	// Helper function for sort()
	// This function is used to build the heap structure 
	// by calling heapifyDown on each non-leaf node in reverse order.
	while (true)
	{
		int left = getLeftChild(idx);
		int right = getRightChild(idx);
		int selected = idx;

		// Only consider elements within the current heap size
		if (left < heapSize && compare(m_Data[selected], m_Data[left]))
			selected = left;

		if (right < heapSize && compare(m_Data[selected], m_Data[right]))
			selected = right;

		if (selected == idx)
			break;

		swap(idx, selected);
		idx = selected;
	}
}

template<typename T>
void PQHeap<T>::sort()
{
	// Build heap (heapify all non-leaf nodes)
	for (int i = m_Size / 2 - 1; i >= 0; i--)
		sortHelper(i, m_Size);

	// Extract elements one by one
	for (int i = m_Size - 1; i > 0; i--)
	{
		// Move current root to end
		swap(0, i);

		// Call heapify on the reduced heap
		sortHelper(0, i);
	}

}

template<typename T>
class MaxPQHeap : public PQHeap<T> {
	bool compare(const T& a, const T& b) const override
	{
		return a > b; 
	}
};

template<typename T>
class MinPQHeap : public PQHeap<T> {
	bool compare(const T& a, const T& b) const override
	{
		return a < b;
	}
};

// Class support by min max 

void PQHeapTest();
// Test function to demonstrate the usage of the heap classes 
