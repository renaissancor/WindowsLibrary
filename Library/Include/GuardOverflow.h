# pragma once 

// GuardOverflow.h 

using std::unordered_map;

namespace GuardOverflow {
	class Manager {
	private: 
		struct AllocInfo {
			uintptr_t page_base;
			size_t total_bytes;
		};
		unordered_map<uintptr_t, AllocInfo> _info;
		size_t _page_size; // PAGE SIZE  
		DWORD _AllocationGranularity; 
		CRITICAL_SECTION _cs; 

	private:
		Manager();
		~Manager();

		void FreeAllMemory() noexcept;
		
	public:
		inline static Manager& GetInstance() noexcept {
			static Manager instance; 
			return instance; 
		}
		
		LPVOID CustomAlloc(size_t bytes_to_alloc) noexcept;
		void CustomFree(LPVOID ptr) noexcept;
		
	};

	inline LPVOID Alloc(size_t bytes_to_alloc) noexcept {
		return Manager::GetInstance().CustomAlloc(bytes_to_alloc);
	}

	inline void Free(LPVOID ptr) noexcept {
		Manager::GetInstance().CustomFree(ptr);
		return; 
	}
}
