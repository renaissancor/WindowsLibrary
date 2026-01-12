#pragma once

// WinMemory.h 

class BaseAllocator {
public:
	static void* Allocate(int size);
	static void  Release(void* ptr);
};

class StompAllocator {
private:
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Allocate(int size);
	static void  Release(void* ptr);
};

template<typename T> 
class stl_allocator {
	using value_type = T; 
public:
	stl_allocator() {} 
	template<typename U> 
	stl_allocator(const stl_allocator<U>&) {} 

	T* allocate(std::size_t count) {
		const std::size_t size = count * sizeof(T);
		return static_cast<T*>(BaseAllocator::Allocate(size));
	}

	void deallocate(T* p, std::size_t count) noexcept {
		BaseAllocator::Release(static_cast<void*>(p)); 
	}
};  

template<typename T>
using Vector = std::vector<T, stl_allocator<T>>; 

template<typename T, typename Container = Vector<T>, 
	typename Pred = std::less<typename Container::value_type>>
using PriorityQueue = std::priority_queue<T, Container, Pred>;

template<typename T>
using Deque = std::deque<T, stl_allocator<T>>; 

template<typename T, typename Container = Deque<T>>
using Queue = std::queue<T, Container>; 

template<typename T, typename Container = Deque<T>>
using Stack = std::stack<T, Container>; 

template<typename K, typename V, typename Pred = std::less<K>>
using Map = std::map<K, V, Pred, stl_allocator<std::pair<const K, V>>>; 

template<typename K, typename Pred = std::less<K>>
using Set = std::set<K, Pred, stl_allocator<K>>;

using String = std::basic_string<char, std::char_traits<char>, stl_allocator<char>>;
using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>, stl_allocator<wchar_t>>; 

template<typename K, typename V, typename Hash = std::hash<K>, 
	typename KeyEqual = std::equal_to<K>>
using HashMap = std::unordered_map<K, V, Hash, KeyEqual, 
	stl_allocator<std::pair<const K, V>>>; 

template<typename K, typename Hash = std::hash<K>, 
	typename KeyEqual = std::equal_to<K>>
using HashSet = std::unordered_set<K, Hash, KeyEqual, stl_allocator<K>>;

template<typename T, typename ...Args> 
inline T* New(Args&& ...args) {
	T* ptr = static_cast<T*>(BaseAllocator::Allocate(sizeof(T)));
	new (ptr) T(std::forward<Args>(args)...); // placement new 
	return ptr; 
}

template<typename T>
inline void Delete(T* ptr) {
	ptr->~T(); // explicit destructor call 
	BaseAllocator::Release(ptr);
}