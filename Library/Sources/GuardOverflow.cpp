#include "pch.h"
#include "GuardOverflow.h" 

// GuardOverflow.cpp 

GuardOverflow::Manager::Manager() 
	: _page_size(4096), _AllocationGranularity(0) {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	_AllocationGranularity = si.dwAllocationGranularity;
	_page_size = si.dwPageSize;
	InitializeCriticalSection(&_cs);
}

GuardOverflow::Manager::~Manager() {
	FreeAllMemory(); 
	DeleteCriticalSection(&_cs);
}	

void GuardOverflow::Manager::FreeAllMemory() noexcept {
	for (auto& pair : _info) {
		VirtualFree((LPVOID)pair.second.page_base, 0, MEM_RELEASE);
	}
	_info.clear();
}

LPVOID GuardOverflow::Manager::CustomAlloc(size_t bytes_to_alloc) noexcept {
	if (bytes_to_alloc == 0) return NULL;
	EnterCriticalSection(&_cs);
	const size_t PAGE = _page_size;
	size_t page_count = (bytes_to_alloc + PAGE - 1) / PAGE + 1; // +1 for guard page
	size_t total_bytes = page_count * PAGE;
	uintptr_t ptr_page_base = (uintptr_t)
		VirtualAlloc(NULL, total_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!ptr_page_base) {
		LeaveCriticalSection(&_cs);
		return NULL;
	}
	uintptr_t ptr_page_guard = ptr_page_base + (page_count - 1) * PAGE;
	DWORD oldProtect = 0;
	if (!VirtualProtect((LPVOID)ptr_page_guard, PAGE, PAGE_NOACCESS, &oldProtect)) {
		VirtualFree((LPVOID)ptr_page_base, 0, MEM_RELEASE);
		LeaveCriticalSection(&_cs);
		return NULL;
	}
	uintptr_t ptr_return = ptr_page_guard - bytes_to_alloc;
	AllocInfo info{ ptr_page_base, total_bytes };
	_info[ptr_return] = info;
	LeaveCriticalSection(&_cs);
	return (LPVOID)ptr_return;
}

// Note: Returned pointer is not page base, it's placed before the guard page.
// You must pass the same pointer to GuardOverflow::Free().

void GuardOverflow::Manager::CustomFree(LPVOID ptr) noexcept {
	if (!ptr) return;
	EnterCriticalSection(&_cs);
	auto it = _info.find((uintptr_t)ptr);
	if (it != _info.end()) {
		VirtualFree((LPVOID)it->second.page_base, 0, MEM_RELEASE);
		_info.erase(it);
	}
	LeaveCriticalSection(&_cs);
}
