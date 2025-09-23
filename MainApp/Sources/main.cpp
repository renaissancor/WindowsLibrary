#include "pch.h"

#include "TestFunctions.h"

int main() {

	// std::cout << "Hello, World!" << std::endl; 
	// TestProfiler();
	// TestNewTracer(); 
	// TestSPSCQueue(); 

	TestMPSCQueue();
	
	// SPMC is not working properly yet 
	// TestSPMCQueue();

    return 0;
}
