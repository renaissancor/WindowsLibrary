#include "pch.h"

#include "TestFunctions.h"

int main() {

	// std::cout << "Hello, World!" << std::endl; 
	// TestProfiler();
	// TestNewTracer(); 
	// TestSPSCQueue(); 

	// TestMPSCQueue();
	
	// SPMC is not working properly yet 
	// TestSPMCQueue();

	// test_cstr_hash_map();
	// test_cstr_hash_map_performance();
	// test_indexed_heap(); 

	// __debugbreak(); 

	// test_guard_overflow(); 

	// test_win_thread(); 

	test_serial_buffer(); 

	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo); 

	printf("Number of Processors: %lu\n", sysInfo.dwNumberOfProcessors);
	printf("Page Size: %lu bytes\n", sysInfo.dwPageSize);
	printf("Processor Type: %lu\n", sysInfo.dwProcessorType);
	printf("Allocation Granularity: %lu bytes\n", sysInfo.dwAllocationGranularity);
	printf("Processor Level: %u\n", sysInfo.wProcessorLevel);
	printf("Processor Revision: %u\n", sysInfo.wProcessorRevision);
	printf("Minimum Application Address: %p\n", sysInfo.lpMinimumApplicationAddress);
	printf("Maximum Application Address: %p\n", sysInfo.lpMaximumApplicationAddress);

    return 0;
}
