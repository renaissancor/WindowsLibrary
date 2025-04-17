#pragma once 

#include <iostream>
#include <cassert>
#include <queue> 

enum TYPE
{
	PARENT, // 0 
	LCHILD, // 1 
	RCHILD, // 2 
	UNDEFINED, // 3 
};

template<typename T1, typename T2>
struct Pair
{
	T1 key; 
	T2 val;
};


template<typename T1, typename T2>
struct Node
{
	Pair<T1, T2> pair;
	Node* ptr[3];

public:
	Node() :
		pair(),
		ptr{ nullptr, nullptr, nullptr }
	{}

	Node(const Pair<T1, T2>& _pair, 
		Node* _Parent = nullptr, 
		Node* _LChild = nullptr, 
		Node* _RChild = nullptr) :
		pair(_pair),
		ptr{ _Parent, _LChild, _RChild}
	{}
};

template<typename T1, typename T2>
Pair<T1, T2> make_pair(const T1& _key, const T2& _val)
{
	Pair<T1, T2> pair;
	pair.key = _key;
	pair.val = _val;
	return pair;
}

template<typename T1, typename T2>
class BSTree
{
private:
	Node<T1, T2>*	m_Root;
	int			    m_Size;

public:
	int  size() { return m_Size; } 
	bool empty() { return m_Size == 0; }

	void clear(); 
	void insert(const Pair<T1, T2>& _pair); 

	class iterator; 
	
	iterator end() { return iterator(this, nullptr); }
	iterator begin() { 
		if (m_Root = nullptr) return iterator(this, nullptr);

		Node<T1, T2>* itrNode = m_Root; 
		while (itrNode->ptr[LCHILD] != nullptr)
		{
			itrNode = itrNode->ptr[LCHILD];
		}

		return iterator(this, itrNode); 
	}

	iterator find(const T1& _key); 

public: 
	BSTree()
		: m_Root(nullptr)
		, m_Size(0)
	{}

	~BSTree()
	{
		clear(); 
	}

	class iterator
	{
	private:
		BSTree<T1, T2>* m_Owner;
		Node<T1, T2>*	m_TargetNode;

	public: 
		bool operator==(const iterator& _Other)
		{
			return (m_Owner == _Other.m_Owner && m_TargetNode == _Other.m_TargetNode);
		}

		bool operator!=(const iterator& _Other)
		{
			return !((*this).operator==(_Other));
		}

		Pair<T1, T2>& operator*()
		{
			assert(m_TargetNode);
			return m_TargetNode->pair;
		}

		iterator& operator ++()
		{
			return *this;
		}

		iterator& operator --()
		{
			return *this; 
		}

	public:
		iterator()
			: m_Owner(nullptr)
			, m_TargetNode(nullptr)
		{}

		iterator(BSTree<T1, T2>* _Ownner, Node<T1, T2>* _Node)
			: m_Owner(_Ownner)
			, m_TargetNode(_Node)
		{}

		~iterator()
		{}

		friend class BSTree; // let iterator access Tree private members 
	};
};


template<typename T1, typename T2>
void BSTree<T1, T2>::clear()
{
	if (m_Root == nullptr) return; // Empty Tree 
	std::queue<Node<T1, T2>*> que;
	que.push(m_Root);

	while (!que.empty())
	{
		Node<T1, T2>* curNode = que.front();
		que.pop();
		if (curNode->ptr[LCHILD] != nullptr)
			que.push(curNode->ptr[LCHILD]);
		if (curNode->ptr[RCHILD] != nullptr)
			que.push(curNode->ptr[RCHILD]);
		delete curNode;
	}

	m_Root = nullptr;
	m_Size = 0; 	
}

template<typename T1, typename T2>
void BSTree<T1, T2>::insert(const Pair<T1, T2> &_pair)
{ 
	Node<T1, T2>* new_node = new Node<T1, T2>(_pair); 

	if(nullptr == m_Root)
	{
		m_Root = new_node; 
		++m_Size; 
		return; 
	}
	
	TYPE direction = UNDEFINED;
	Node<T1, T2>* itr_node = m_Root; 
	while (nullptr != itr_node)
	{
		if (new_node->pair.key < itr_node->pair.key)
		{
			direction = LCHILD; 
		}
		else if (new_node->pair.key > itr_node->pair.key) {
			direction = RCHILD;
		} 
		else {
			assert(nullptr); // ERROR if duplicated key is input 
			delete new_node; 
			return; 
		} // if else direction select 

		if (nullptr == itr_node->ptr[direction])
		{
			itr_node->ptr[direction] = new_node; 
			new_node->ptr[PARENT] = itr_node; 
			break; 
		} 
			
		else
		{
			itr_node = itr_node->ptr[direction]; 
		}

	} // While loop ends here 
	++m_Size; 
	
}


template<typename T1, typename T2>
typename BSTree<T1, T2>::iterator BSTree<T1, T2>::find(const T1& _key)
{
	if (m_Root == nullptr) return end(); // Empty Tree 
	Node<T1, T2>* itr_node = m_Root;
	
	while (itr_node)
	{
		if (_key == itr_node->pair.key)
		{
			return iterator(this, itr_node); 
		}
		else if (_key < itr_node->pair.key) {
			itr_node = itr_node->ptr[LCHILD]; 
		}
		else {
			itr_node = itr_node->ptr[RCHILD]; 
		}
	}

	return end(); // Not Found
}


void BSTreeTest(); 

// Ctrl K C to comment 
// Ctrl K U to uncomment 
//++operator of the in order traversal
//first check whether right child exist then
//check whether the right child has left children
//Go to the parent, and check whether parent is left children of the parent node
//
//1. Right Child ? go until RCHild is nullptr
//2. check whether you are LChild of the parent
//3. check whether your parent is LChild of Parent parent
//
//BST iterator rule
//if RChild does not exist check parent left child
//check whether parent node is parent parent node's left child 