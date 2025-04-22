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
	bool HasLChild() { return nullptr != ptr[LCHILD]; }
	bool HasRChild() { return nullptr != ptr[RCHILD]; }

	bool IsRoot() { return nullptr == this->ptr[PARENT]; }
	bool IsLeft() { return nullptr == this->ptr[LCHILD] && this->ptr[RCHILD]; }
	bool IsFull() { return this->ptr[LCHILD] && this->ptr[RCHILD] }; 

	bool IsLChild(){ return(ptr[PARENT] && ptr[PARENT]->ptr[LCHILD] == this); }
	bool IsRChild() { return(ptr[PARENT] && ptr[PARENT]->ptr[RCHILD] == this); }

	Node* GetNode(TYPE _TYPE)
	{
		assert(_TYPE != UNDEFINED); 
		return (this->ptr[_TYPE]); 
	}

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

	Node<T1, T2>* getInOrderSuccessor(Node<T1, T2>* _Node); 
	Node<T1, T2>* getInOrderPredecessor(Node<T1, T2>* _Node);

	class iterator; 
	
	iterator begin() { 

		Node<T1, T2>* itrNode = m_Root; 
		if (m_Root == nullptr) return iterator(this, nullptr);

		while (itrNode->ptr[LCHILD] != nullptr)
		{
			itrNode = itrNode->ptr[LCHILD];
		}

		return iterator(this, itrNode); 
	}

	iterator end() { return iterator(this, nullptr); }

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
			m_TargetNode = m_Owner->getInOrderSuccessor(m_TargetNode); 
			return *this; 
		}

		iterator& operator--()
		{
			if (m_TargetNode == nullptr) // we're at end()
			{
				if (m_Owner->empty())
				{
					return *this; // still at end() for empty tree
				}
				// Go to rightmost node (max)
				m_TargetNode = m_Owner->m_Root;
				while (m_TargetNode->ptr[RCHILD])
				{
					m_TargetNode = m_TargetNode->ptr[RCHILD];
				}
			}
			else
			{
				m_TargetNode = m_Owner->getInOrderPredecessor(m_TargetNode);
			}

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


template<typename T1, typename T2>
Node<T1, T2>* BSTree<T1,T2>::getInOrderSuccessor(Node<T1, T2>* _Node)
{
	assert(_Node); // If input _Node == nullptr return 
	Node<T1, T2>* next_node = nullptr;
	if (_Node->HasRChild())
	{
		next_node = _Node->GetNode(RCHILD);
		while (next_node->GetNode(LCHILD)) { next_node  = next_node->GetNode(LCHILD); }
	}

	else
	{
		Node<T1, T2>* itr_node = _Node;
		while (!(itr_node->IsRoot()))
		{
			if (itr_node->IsLChild()) {
				next_node = itr_node->GetNode(PARENT);
				break; 
			}
			else {
				itr_node = itr_node->GetNode(PARENT); 
			}
		}
	}
	return next_node;
}

template<typename T1, typename T2>
Node<T1, T2>* BSTree<T1, T2>::getInOrderPredecessor(Node<T1, T2>* _Node)
{
	if (_Node == nullptr) return nullptr;

	// Case 1: Has left child → predecessor is the rightmost of left subtree
	if (_Node->HasLChild())
	{
		Node<T1, T2>* pred_node = _Node->ptr[LCHILD];
		while (pred_node->ptr[RCHILD])
		{
			pred_node = pred_node->ptr[RCHILD];
		}
		return pred_node;
	}

	// Case 2: No left child → walk up until we find a right child
	Node<T1, T2>* parent = _Node->ptr[PARENT];
	while (parent && _Node == parent->ptr[LCHILD])
	{
		_Node = parent;
		parent = parent->ptr[PARENT];
	}

	return parent; // Could be nullptr (i.e., _Node was the leftmost)
}



template<typename T1, typename T2>
typename BSTree<T1, T2>::iterator erase(typename BSTree<T1, T2>::iterator _iter)
{
	assert(nullptr != _iter.m_TargetNode); // Check whether the iterator is valid
	Node<T1,T2>* pSuccessor = nullptr; 
	if (_iter->m_TargetNode->IsLeaf())
	{
		// Delete Node 
		// 1. If delected node is root 
		// 2. if delected node is not root 
		// If parent node is not pointing the node targeted to be deleted 
		if (_iter->m_TargetNode->IsRoot())
		{
			delete _iter->m_TargetNode;
			m_Root = nullptr;
		}
		else
		{
			Node<T1, T2>* parent = _iter->m_TargetNode->ptr[PARENT];
			if (_iter->m_TargetNode->IsLChild())
			{
				parent->ptr[LCHILD] = nullptr;
			}
			else
			{
				parent->ptr[RCHILD] = nullptr;
			}
			delete _iter->m_TargetNode;
		}
	
	}
	else if (_iter->m_TargetNode->IsFull())
	{
		// Since both sides are full find either predecessor or successor and replace 
		// This case find in order successor to replace the original position 
	}
	else { // Only one node exists  

	}
	// Return successor node after the deleted node as iterator and return 

	delete _iter->m_TargetNode; 
	--this->m_Size; 
}

// Even after erase iterator in order traversal 


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