#pragma once

// WinSharedPtr.h 

#include "WinAtomic.h"

namespace Win {
	class RefCount {
	protected:
		Atomic32 _counter;
	public:
		RefCount() noexcept : _counter(1) {}
		virtual ~RefCount() {}

		inline int GetRefCount() const noexcept { return _counter.Load(); }
		inline void AddRef() noexcept { _counter.Increment(); }
		inline int ReleaseRef() noexcept {
			int count = _counter.Decrement();
			if (count == 0) delete this;
			return count;
		}
	};

	template<typename T>
	class SharedPtr {
	private:
		T* _ptr = nullptr;

	private:
		inline void Set(T* ptr) noexcept {
			_ptr = ptr;
			if (_ptr) _ptr->AddRef(); 
		}

		inline void Release() noexcept {
			if (_ptr) {
				_ptr->ReleaseRef();
				_ptr = nullptr;
			}
		}

	public:
		inline SharedPtr() {} 
		inline SharedPtr(T* ptr) noexcept { Set(ptr); }
		inline SharedPtr(const SharedPtr& other) noexcept { Set(other._ptr); }
		inline SharedPtr(SharedPtr&& other) noexcept : _ptr(other._ptr)
			{ other._ptr = nullptr; }
		~SharedPtr() noexcept { Release(); } 
		template<typename U>
		inline SharedPtr(const SharedPtr<U>& other) noexcept
			{ Set(static_cast<T*>(other._ptr); }

		inline SharedPtr& operator=(const SharedPtr& other) noexcept 
		{ 
			if (this->_ptr != &other._ptr) {
				Release();
				Set(other._ptr);
			}
			return *this; 
		}
		inline SharedPtr& operator=(SharedPtr&& other) noexcept 
		{ 
			if (this->_ptr != &other._ptr) {
				Release();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this; 
		}

	public:
		inline bool IsNull() const noexcept { return _ptr == nullptr; } 
		inline bool operator==(const SharedPtr& other) const noexcept { return _ptr == other._ptr; }
		inline bool operator==(T* ptr) const noexcept { return _ptr == ptr; }
		inline bool operator!=(const SharedPtr& other) const noexcept { return _ptr != other._ptr; }
		inline bool operator!=(T* ptr) const noexcept { return _ptr != ptr; }
		inline bool operator<(const SharedPtr& other) const noexcept { return _ptr < other._ptr; }
		inline bool operator>(const SharedPtr& other) const noexcept { return _ptr > other._ptr; }
		inline T* operator*() const noexcept { return _ptr; } 
		inline const T* operator*() const noexcept { return _ptr; }
		inline operator T* () const noexcept { return _ptr; } 
		inline const T* operator->() const noexcept { return _ptr; } 
	};

} // namespace Win 

