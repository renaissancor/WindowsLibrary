#include "pch.h"

#include "SPSCQueue.h"
#include "TestFunctions.h" 

struct SPSCParam {
	SPSCQueue<int>* queue;
	const int total_items;
	std::vector<int>* consumed_items;
};

static unsigned int __stdcall SPSCConsumerThread(void* arg) noexcept {
	SPSCParam* param = static_cast<SPSCParam*>(arg);
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

static unsigned int __stdcall SPSCProducerThread(void* arg) noexcept {
	SPSCParam* param = static_cast<SPSCParam*>(arg);
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
    const int total_items = 1000000;

    for (int round = 0; round < 5; ++round) {
        SPSCQueue<int> queue(capacity);
        std::vector<int> consumed_items;
        consumed_items.reserve(total_items);

        SPSCParam* param = new SPSCParam{ &queue, total_items, &consumed_items };

        HANDLE consumerHandle = (HANDLE)_beginthreadex(
            nullptr, 0, &SPSCConsumerThread, param, 0, nullptr);
        HANDLE producerHandle = (HANDLE)_beginthreadex(
            nullptr, 0, &SPSCProducerThread, param, 0, nullptr);
        if (!consumerHandle || !producerHandle) {
            std::cerr << "Error: Unable to create threads.\n";
            return;
        }
        
        WaitForSingleObject(consumerHandle, INFINITE);
        WaitForSingleObject(producerHandle, INFINITE);

        CloseHandle(consumerHandle);
        CloseHandle(producerHandle);

        bool success = true;

        if (consumed_items.size() != total_items) {
            std::cout << "[Round " << round << "] Error: Size mismatch. Expected "
                << total_items << ", Got " << consumed_items.size() << "\n";
            success = false;
        }

        for (int i = 0; i < (int)consumed_items.size(); ++i) {
            if (consumed_items[i] != i) {
                std::cout << "[Round " << round << "] Error: Order mismatch at index "
                    << i << ". Expected " << i << ", Got " << consumed_items[i] << "\n";
                success = false;
                break;
            }
        }

        if (success) {
            std::cout << "[Round " << round << "] Test Passed! "
                << total_items << " items processed correctly.\n";
        }
        else {
            std::cout << "[Round " << round << "] Test Failed!\n";
        }

        delete param;
    }
}
