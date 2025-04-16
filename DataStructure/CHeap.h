#include <assert.h>

// Heap is a Binary Tree with specific ordering (min or max) 
// properties, typically implemented using a dynamic array.
// C++ STL provides heap-based algorithms and containers:
//   - std::make_heap, std::push_heap, std::pop_heap
//   - std::priority_queue is a heap-based container.
// Note: OS heap (dynamic memory) is unrelated to this data structure.

template<typename T>
class Heap {
private:
	T*	 m_Data;
	int	 m_Capacity;
	int	 m_Size;

protected: 

	virtual bool compare(const T& a, const T& b) const = 0; 

	int parentIdx(int i)
	{
		return (i - 1) / 2; 
	}

	int leftChildIdx(int i)
	{
		return 2 * i + 1; 
	}

	int rightChildIdx(int i)
	{
		return 2 * i + 2; 
	}

public:


	const T& top() const
	{
		return *m_Data; 
	}

	bool empty() const
	{
		return !(m_Size); 
	}

	int size() const
	{
		return m_Size; 
	}


public: // function declaration 
	Heap(){}
	~Heap(){}
	void reserve(int _Capacity); 
	void resize(int _Size); 

	void heapifyUp(int index);
	void heapifyDown(int index);
	void swap(int i, int j);

	void push(const T& _Data);
	T pop();
};


template<typename T>
Heap<T>::Heap()
	: m_Data(nullptr)
	, m_Capacity(0)
	, m_Size(0)
{
}

template<typename T>
Heap<T>::~Heap()
{
	delete[] m_Data;
}

template<typename T>
void Heap<T>::reserve(int _Capacity)
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
void Heap<T>::resize(int _Size)
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
void Heap<T>::push(const T& _Data)
{
	if (m_Size == m_Capacity) {
		reserve(m_Capacity == 0 ? 1 : m_Capacity * 2);
	}

	m_Data[m_Size++] = _Data; 
}


template<typename T>
T Heap<T>::pop()
{
	assert(!(this->empty()));

	T root = m_Data[0]; 
	m_Data[0] = m_Data[--m_Size]; 

	return m_Data[0]; 
}

template<typename T>
class MaxHeap : public Heap {
	bool compare(const T& a, const T& b) const override
	{
		return a > b; 
	}
};

template<typename T>
class MinHeap : public Heap {
	bool compare(const T& a, const T& b) const override
	{
		return a < b;
	}
};
