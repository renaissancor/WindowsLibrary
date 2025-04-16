#pragma once

#include <assert.h>

// 클래스 템플릿
template<typename T>
class CArray
{
private:
	T* m_Data;
	int		m_Capacity;
	int		m_CurSize;

public:
	void push_back(const T& _Data);
	void reserve(int _Capacity);
	void resize(int _Size);
	void clear() { m_CurSize = 0; }

	class iterator;
	iterator begin()
	{
		if (m_CurSize == 0)
			return end();
		else
			return iterator(this, 0);
	}

	iterator end()
	{
		return iterator(this, -1);
	}

	// 맨 마지막 데이터 삭제
	void pop_back()
	{
		// 데이터가 1개도 없는데, 맨 마지막을 지우라고 한 경우
		assert(m_CurSize);
		--m_CurSize;
	}

	// 반환타입 iterator 는, 삭제된 다음 요소를 가리키는 iterator 
	// 입력으로 들어온 itetator 가 가리키는 요소를 삭제시키고,
	// 입력으로 들어온 iterator 는 더이상 사용할 수 없는 상태로 만들어주어야한다.
	iterator erase(iterator& _TargetIter);


	void swap(CArray& _Other)
	{
		T* pData = m_Data;
		m_Data = _Other.m_Data;
		_Other.m_Data = pData;

		int Temp = m_Capacity;
		m_Capacity = _Other.m_Capacity;
		_Other.m_Capacity = Temp;

		Temp = m_CurSize;
		m_CurSize = _Other.m_CurSize;
		_Other.m_CurSize = Temp;
	}


	int size() { return m_CurSize; }
	int capacity() { return m_Capacity; }

	T& at(int _idx)
	{
		assert(_idx < m_CurSize);
		return m_Data[_idx];
	}

	T& operator[](int _idx)
	{
		assert(_idx < m_CurSize);
		return m_Data[_idx];
	}

	CArray& operator =(const CArray<T>& _Other);


public:
	CArray();
	~CArray();


	// CArray 의 private 접근 가능
	class iterator
	{
	private:
		CArray<T>* m_Owner;
		int			m_Idx;

		// m_Owner == nullptr && m_Idx == -1  ==> 아무것도 안가리키기고 있는 iterator
		// m_Onwer != nullptr && m_Idx == -1  ==> 특정 컨테이너의 end 지점을 가리키고 있는 상태

	public:
		T& operator*()
		{
			return m_Owner->m_Data[m_Idx];
		}

		iterator& operator ++()
		{
			// End Iterator 에 ++ 을 호출한 경우
			assert(m_Idx != -1);

			++m_Idx;

			// 컨테이너가 보유한 데이터 개수를 초과해서 가리키게 됨 ==> End Iterator
			if (m_Owner->m_CurSize <= m_Idx)
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

		iterator& operator --()
		{
			// Begin iterator 에 -- 를 호출한 경우
			assert(m_Idx);

			// End iterator 에 -- 를 호출한 경우
			if (-1 == m_Idx)
			{
				// 컨테이너가 데이터를 소유하고 있지 않으면, 마지막 데이터가 존재하지 않는다.
				assert(m_Owner->m_CurSize);

				// End 에서 이전으로 가려면, 마지막 데이터를 가리켜야한다.
				m_Idx = m_Owner->m_CurSize - 1;
			}
			else
			{
				--m_Idx;
			}

			return *this;
		}

		iterator operator--(int)
		{
			iterator copyiter(*this);

			--(*this);

			return copyiter;
		}

		bool operator == (const iterator& _Other)
		{
			if (m_Owner == _Other.m_Owner && m_Idx == _Other.m_Idx)
				return true;

			return false;
		}

		bool operator !=(const iterator& _Other)
		{
			return !((*this) == _Other);
		}



	public:
		iterator()
			: m_Owner(nullptr)
			, m_Idx(-1)
		{
		}

		iterator(CArray<T>* _Owner, int _Idx)
			: m_Owner(_Owner)
			, m_Idx(_Idx)
		{
		}

		~iterator()
		{
		}
		friend class CArray;
	};
};


template<typename T>
CArray<T>::CArray()
	: m_Data(nullptr)
	, m_Capacity(0)
	, m_CurSize(0)
{
}

template<typename T>
CArray<T>::~CArray()
{
	delete[] m_Data;
}

template<typename T>
void CArray<T>::push_back(const T& _Data)
{
	// 현재 힙공간에 데이터가 꽉 찬 경우
	if (m_Capacity <= m_CurSize)
	{
		reserve(m_Capacity * 2);
	}

	m_Data[m_CurSize++] = _Data;
}

template<typename T>
void CArray<T>::reserve(int _Capacity)
{
	if (0 == _Capacity)
		_Capacity = 1;

	// 더 넓은 공간으로 데이터를 이전해야한다.
	T* pNewData = new T[_Capacity];

	for (int i = 0; i < m_CurSize; ++i)
	{
		pNewData[i] = m_Data[i];
	}

	delete[] m_Data;

	m_Data = pNewData;

	m_Capacity = _Capacity;
}

template<typename T>
void CArray<T>::resize(int _Size)
{
	// resize 수치가 기존크기보다 더 크면
	if (m_CurSize < _Size)
	{
		reserve(_Size);

		// 추가된 공간을 기본값으로 채움
		for (int i = 0; i < _Size - m_CurSize; ++i)
		{
			m_Data[m_CurSize + i] = T();
		}
	}

	// 변경된 사이즈로 갱신
	m_CurSize = _Size;
}

template<typename T>
typename CArray<T>::iterator CArray<T>::erase(iterator& _TargetIter)
{
	// 1. 삭제된 다음을 가리키는 iterator 를 만들어서 반환해줘야 한다.
	// 2. 배열이 보유한 데이터보다 더 큰 인덱스를 가리키는 iterator 가 입력으로 들어온 경우
	// 3. iterator 가 end iterator 인 경우
	assert((this == _TargetIter.m_Owner)
		&& (_TargetIter.m_Idx < m_CurSize)
		&& (_TargetIter.m_Idx != -1));

	// 삭제된 다음을 가리키는 iterator 를 만들어둔다.
	iterator NextIter(this, _TargetIter.m_Idx);

	// 삭제된 위치로 뒤에있는 데이터들이 한칸씩 당겨져야 한다.
	int MoveCount = m_CurSize - (_TargetIter.m_Idx + 1);
	for (int i = 0; i < MoveCount; ++i)
	{
		m_Data[i + _TargetIter.m_Idx] = m_Data[i + 1 + _TargetIter.m_Idx];
	}

	--m_CurSize;

	// 입력으로 사용된 iterator 의 상태를 아무것도 가리키지 않는, 쓸 수 없는 상태로 만든다.
	_TargetIter.m_Owner = nullptr;
	_TargetIter.m_Idx = -1;

	return NextIter;
}



template<typename T>
CArray<T>& CArray<T>::operator=(const CArray<T>& _Other)
{
	if (_Other.m_CurSize == 0)
	{
		m_CurSize = 0;
		return *this;
	}

	// 기존 공간을 삭제한다.
	if (nullptr != m_Data)
		delete[] m_Data;

	// 원본 배열이 보유한 데이터 개수만큼 동적할당을 한다.
	m_Data = new int[_Other.m_CurSize];

	// 원본배열이 보유한 데이터를, 새로 할당한 공간에 복사받는다.
	for (int i = 0; i < _Other.m_CurSize; ++i)
	{
		m_Data[i] = _Other.m_Data[i];
	}

	// Capacity, CurSize 갱신
	m_Capacity = _Other.m_CurSize;
	m_CurSize = _Other.m_CurSize;

	return *this;
}