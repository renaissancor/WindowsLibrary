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

	private:
		Manager(): _page_size(4096), _AllocationGranularity(0) {
			SYSTEM_INFO si; 
			GetSystemInfo(&si); 
			_AllocationGranularity = si.dwAllocationGranularity; 
			_page_size = si.dwPageSize;
		}

		void FreeAllMemory() noexcept {
			for (auto& pair : _info) {
				VirtualFree((LPVOID)pair.second.page_base, 0, MEM_RELEASE);
			}
			_info.clear();
		}

		~Manager() {
			FreeAllMemory(); 
		}

	public:
		inline static Manager& GetInstance() noexcept {
			static Manager instance; 
			return instance; 
		}
	
		LPVOID Alloc(size_t bytes_to_alloc) noexcept {
			if (bytes_to_alloc == 0) return NULL;

			const size_t PAGE = _page_size;
			size_t page_count = (bytes_to_alloc + PAGE - 1) / PAGE + 1; // +1 for guard page
			size_t total_bytes = page_count * PAGE;

			uintptr_t ptr_page_base = (uintptr_t)
				VirtualAlloc(NULL, total_bytes, MEM_COMMIT, PAGE_READWRITE);
			if (!ptr_page_base) return NULL;

			uintptr_t ptr_page_guard = ptr_page_base + (page_count - 1) * PAGE;
			DWORD oldProtect = 0;
			VirtualProtect((LPVOID)ptr_page_guard, PAGE, PAGE_NOACCESS, &oldProtect);

			uintptr_t ptr_return = ptr_page_guard - bytes_to_alloc;
			AllocInfo info{ ptr_page_base, total_bytes };
			_info[ptr_return] = info;

			return (LPVOID)ptr_return;
		}

		void Free(LPVOID ptr) noexcept {
			auto it = _info.find((uintptr_t)ptr);
			if (it != _info.end()) {
				VirtualFree((LPVOID)it->second.page_base, 0, MEM_RELEASE);
				_info.erase(it);
			}
		}
	};

	inline LPVOID Alloc(size_t bytes_to_alloc) noexcept {
		return Manager::GetInstance().Alloc(bytes_to_alloc); 
	}

	void Free(LPVOID ptr) noexcept {
		Manager::GetInstance().Free(ptr); 
		return; 
	}
}
