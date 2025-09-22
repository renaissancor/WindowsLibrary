#pragma once

#include <assert.h>

template<typename T>
struct tNode
{
	T		m_Data;	// 노드안에 저장된 데이터
	tNode* m_Next;	// 다음 노드를 가리킴
	tNode* m_Prev; // 이전노드를 가리킴


public:
	tNode()
		: m_Data()
		, m_Next(nullptr)
		, m_Prev(nullptr)
	{
	}

	tNode(const T& _Data, tNode* _Next = nullptr, tNode* _Prev = nullptr)
		: m_Data(_Data)
		, m_Next(_Next)
		, m_Prev(_Prev)
	{
	}
};

// Double Linked List DLList 

template<typename T>
class DLList
{
private:
	tNode<T>* m_Head;	// 시작데이터를 가지고있는 노드를 가리킴
	tNode<T>* m_Tail; // 끝에 있는 데이터를 가지고있는 노드를 가리킴
	int			m_Size; // 보유하고 있는 데이터 개수

public:
	int size() { return m_Size; }
	void clear();

	void push_back(const T& _Data);
	void push_front(const T& _Data);

	T& front() { assert(m_Head); return m_Head->m_Data; }
	T& back() { assert(m_Tail); return m_Tail->m_Data; }

	// 가장 첫번째 데이터를 삭제
	void pop_front();

	// 가장 마지막 데이터를 삭제
	void pop_back();

	void reverse()
	{
		reverse(m_Head);
	}

private:
	// 재귀함수 구조를 활용할 것
	void reverse(tNode<T>* _Node)
	{
		if (nullptr != _Node->m_Next)
		{
			reverse(_Node->m_Next);
		}

		// 가장 마지막 노드에 도달하면
		if (_Node->m_Next == nullptr)
		{
			// List 가 가리키고있던 Head 랑 Tail 을 스�판蠻娩�.
			tNode<T>* pTemp = m_Head;
			m_Head = m_Tail;
			m_Tail = pTemp;
		}

		tNode<T>* pTemp = _Node->m_Next;
		_Node->m_Next = _Node->m_Prev;
		_Node->m_Prev = pTemp;
	}


public:
	class iterator;
	iterator begin() { return iterator(this, m_Head); }
	iterator end() { return iterator(this, nullptr); }
	iterator erase(iterator& _iter);




public:
	DLList()
		: m_Head(nullptr)
		, m_Tail(nullptr)
		, m_Size(0)
	{
	}

	~DLList()
	{
		clear();
	}

public:
	class iterator
	{
	private:
		DLList<T>* m_Owner;		// 데이터를 저장 및 관리하는 list 객체
		tNode<T>* m_TargetNode;	// list 객체가 보유하고 있는 데이터를, 저장하고있는 노드

		// m_Owner == nullptr && m_TargetNode == nullptr ==> 아무것도 가리키지 않는 상태
		// m_Owner != nullptr && m_TargetNode == nullptr ==> List 보유 데이터의 마지막 다음을 가리키는 end iterator 상태

	public:
		bool operator ==(const iterator& _Other)
		{
			if (m_Owner == _Other.m_Owner && m_TargetNode == _Other.m_TargetNode)
				return true;
			return false;
		}

		bool operator != (const iterator& _Other)
		{
			return !((*this) == _Other);
		}

		iterator& operator++()
		{
			// iterator 가 아무것도 안가리키고 있거나, 
			// end iterator 상태면 다음을 가리킬 수 없다.
			assert(m_TargetNode);

			m_TargetNode = m_TargetNode->m_Next;

			return *this;
		}

		iterator operator++(int)
		{
			iterator copyiter(*this);
			++(*this);
			return copyiter;
		}


		iterator& operator--()
		{
			// Begin Iterator 에 -- 를 호출한 경우
			assert(m_Owner->m_Head != m_TargetNode);

			// End iterator 에 -- 를 호출한 경우
			if (nullptr == m_TargetNode)
			{
				// 컨테이너에 데이터가 하나도 없다(이전으로 갈 마지막 노드가 존재하지 않는다.)
				assert(m_Owner->m_Tail);

				m_TargetNode = m_Owner->m_Tail;
			}

			else
			{
				m_TargetNode = m_TargetNode->m_Prev;
			}

			return *this;
		}

		iterator operator--(int)
		{
			iterator copyiter(*this);
			--(*this);
			return copyiter;
		}

		T& operator*()
		{
			assert(m_TargetNode);
			return m_TargetNode->m_Data;
		}

	public:
		iterator()
			: m_Owner(nullptr)
			, m_TargetNode(nullptr)
		{
		}
		iterator(DLList<T>* _Owner, tNode<T>* _Node)
			: m_Owner(_Owner)
			, m_TargetNode(_Node)
		{
		}
		~iterator()
		{
		}

		// DLList 클래스 쪽에서 iterator 의 private 까지 접근 가능하게 허용해준다.
		friend class DLList;
	};
};


template<typename T>
void DLList<T>::push_back(const T& _Data)
{
	tNode<T>* pNewNode = new tNode<T>(_Data);

	// 데이터가 첫번째라면
	if (nullptr == m_Head)
	{
		// 데이터가 1개로 유일한 경우, 이 데이터는 시작이자 끝으로 볼 수 있다.
		m_Head = pNewNode;
		m_Tail = pNewNode;
	}

	// 데이터라 1개 이상 이미 있다.
	else
	{
		// List 의 Tail 이 가리키는 노드랑, 새로생긴 노드를 서로 연결해준다.
		m_Tail->m_Next = pNewNode;
		pNewNode->m_Prev = m_Tail;

		// 새로 생성된 노드를 새로운 Tail 로 지정한다.
		m_Tail = pNewNode;
	}

	++m_Size;
}

template<typename T>
void DLList<T>::push_front(const T& _Data)
{
	// 입력된 데이터를 저장할 노드를 생성하고,
	// 생성된 노드는 기존 Head 노드를 자신의 다음으로 지정한다.
	tNode<T>* pNewNode = new tNode<T>(_Data, m_Head);

	// 새로 생성된 노드를 새로운 헤드로 지정한다.
	m_Head = pNewNode;

	// List 가 데이터를 1개 이상 원래 가지고 있었다면,
	if (1 <= m_Size)
	{
		// 기존 헤드노드가, 새로 생성된 노드를 자신의 이전으로 가리킨다.
		pNewNode->m_Next->m_Prev = pNewNode;
	}

	// 데이터 카운팅
	++m_Size;
}

template<typename T>
inline void DLList<T>::pop_front()
{
	// 데이터가 하나도 없는데 pop_front를 호출한 경우
	assert(m_Head);

	// 데이터가 2개 이상인 경우,
	if (nullptr != m_Head->m_Next)
	{
		// 두번째 노드로 접근해서, 이전노드를 가리키기지 않게 한다.
		m_Head->m_Next->m_Prev = nullptr;
	}

	// 데이터가 1개인경우
	else
	{
		m_Tail = nullptr;
	}

	tNode<T>* pHead = m_Head;
	m_Head = m_Head->m_Next;
	delete pHead;

	// 데이터 개수 카운팅
	--m_Size;
}

template<typename T>
void DLList<T>::pop_back()
{
	// 데이터가 하나도 없는데 pop_back를 호출한 경우
	assert(m_Size);

	if (1 == m_Size)
	{
		pop_front();
		return;
	}

	// 마지막 노드를 가리키는 m_Tail 의 값을 하나 이전 노드를 가리키게 한다.	
	m_Tail = m_Tail->m_Prev;

	// 원래 마지막 노드를 삭제한다.
	delete m_Tail->m_Next;

	// 새로운 마지막 노드가 다음을 가리키는 포인터를 nullptr 로 막는다.
	m_Tail->m_Next = nullptr;

	--m_Size;
}

template<typename T>
typename DLList<T>::iterator DLList<T>::erase(iterator& _iter)
{
	// 입력으로 들어온 iterator 가 가리키는 대상을 삭제하고
	// 삭제된 다음을 가리키는 itetator 를 만들어서 반환해준다.

	// iterator 가 가리키는 데이터를 소유하고있는 컨테이너가 erase 호출자 본인이 아니다.
	assert(_iter.m_Owner == this);


	// 삭제시킬 노드 다음 노드를 가리키는 iterator 를 만들어둔다.
	iterator NextIter = iterator(this, _iter.m_TargetNode->m_Next);

	// 첫번째 노드를 제거
	if (m_Head == _iter.m_TargetNode)
	{
		pop_front();
	}

	// 마지막 노드를 제거
	else if (m_Tail == _iter.m_TargetNode)
	{
		pop_back();
	}

	// 중간에 있는 노드를 제거
	else
	{
		_iter.m_TargetNode->m_Prev->m_Next = _iter.m_TargetNode->m_Next;
		_iter.m_TargetNode->m_Next->m_Prev = _iter.m_TargetNode->m_Prev;
		delete _iter.m_TargetNode;
		--m_Size;
	}

	return NextIter;
}


template<typename T>
void DLList<T>::clear()
{
	tNode<T>* pNode = m_Head;
	for (int i = 0; i < m_Size;++i)
	{
		tNode<T>* pNext = pNode->m_Next;
		delete pNode;
		pNode = pNext;
	}

	m_Head = nullptr;
	m_Tail = nullptr;
	m_Size = 0;
}