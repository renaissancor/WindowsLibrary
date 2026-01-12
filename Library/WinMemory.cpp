#include "pch.h"

#include "WinMemory.h"

// WinMemory.cpp

void* BaseAllocator::Allocate(int size) {
	return malloc(size);
}

void BaseAllocator::Release(void* ptr) {
	free(ptr);
}

void* StompAllocator::Allocate(int size) {
	const uintptr_t pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const uintptr_t dataOffset = pageCount * PAGE_SIZE - size;
	void* basePtr = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, 
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); 
	return static_cast<void*>(static_cast<char*>(basePtr) + dataOffset); 
}

void StompAllocator::Release(void* ptr) {
	const uintptr_t thisPtr = reinterpret_cast<uintptr_t>(ptr);
	const uintptr_t basePtr = thisPtr - (thisPtr % PAGE_SIZE); 
	::VirtualFree(reinterpret_cast<LPVOID>(basePtr), 0, MEM_RELEASE); 
}