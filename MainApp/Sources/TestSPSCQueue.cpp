#include "pch.h"

#include "SPSCQueue.h"
#include "TestFunctions.h" 

struct ThreadParam {
	SPSCQueue<int>* queue;
	const int total_items;
	std::vector<int>* consumed_items;
};

static unsigned int __stdcall ConsumerThread(void* arg) noexcept {
	ThreadParam* param = static_cast<ThreadParam*>(arg);
	SPSCQueue<int>* queue = param->queue;
	int received_count = 0;
	while (received_count < param->total_items) {
		int item;
		if (queue->Pop(item)) {
			param->consumed_items->push_back(item);
			++received_count;
		}
		else {
			Sleep(0); 
		}
	}
	return 0;
}

static unsigned int __stdcall ProducerThread(void* arg) noexcept {
	ThreadParam* param = static_cast<ThreadParam*>(arg);
	SPSCQueue<int>* queue = param->queue;
	for (int i = 0; i < param->total_items; ++i) {
		while (!queue->Push(i)) {
			Sleep(0); 
		}
	}
	return 0;
}

void TestSPSCQueue() {
	const int capacity = 1024;
	SPSCQueue<int> queue(capacity);

	const int total_items = 1000000;
	std::vector<int> consumed_items;

	ThreadParam *param = new ThreadParam{ &queue, total_items, &consumed_items };

	HANDLE ConsumerThreadHandle = (HANDLE)_beginthreadex(
		nullptr, 0, &ConsumerThread, param, 0, nullptr);
	HANDLE ProducerThreadHandle = (HANDLE)_beginthreadex(
		nullptr, 0, &ProducerThread, param, 0, nullptr);
	if (ConsumerThreadHandle == nullptr || ProducerThreadHandle == nullptr) {
		fprintf_s(stderr, "Error creating threads.\n");
		return;
	}

	WaitForSingleObject(ConsumerThreadHandle, INFINITE);
	WaitForSingleObject(ProducerThreadHandle, INFINITE); 

	CloseHandle(ConsumerThreadHandle);
	CloseHandle(ProducerThreadHandle); 

	bool success = true;
	if (consumed_items.size() != total_items) {
		std::cout << "Error: Received item count mismatch! Expected: " << total_items
			<< ", Got: " << consumed_items.size() << std::endl;
		success = false;
	}

	if (success) {
		for (int i = 0; i < total_items; ++i) {
			if (consumed_items[i] != i) {
				std::cout << "Error: Data order mismatch at index " << i
					<< ". Expected: " << i << ", Got: " << consumed_items[i] << std::endl;
				success = false;
				break;
			}
		}
	}

	if (success) {
		std::cout << "SPSC Queue Test Passed! All " << total_items << " items processed correctly." << std::endl;
	}
	else {
		std::cout << "SPSC Queue Test Failed!" << std::endl;
	}

	delete param; 
}