#include "pch.h"

#include "SPMCQueue.h"

struct SPMCParam {
    SPMCQueue<int>* queue;
    int total_items;
    std::vector<int>* local_consumed;
};

static unsigned int __stdcall SPMCConsumerThread(void* arg) noexcept {
    SPMCParam* param = static_cast<SPMCParam*>(arg);
    SPMCQueue<int>* queue = param->queue;

    while (true) {
        int item;
        if (queue->Pop(item)) {
            param->local_consumed->push_back(item);
        }
        else {
            if (param->local_consumed->size() >= (size_t)param->total_items) break;
            Sleep(0);
        }
    }
    return 0;
}

static unsigned int __stdcall SPMCProducerThread(void* arg) noexcept {
    SPMCParam* param = static_cast<SPMCParam*>(arg);
    SPMCQueue<int>* queue = param->queue;

    for (int i = 0; i < param->total_items; ++i) {
        while (!queue->Push(i)) {
            Sleep(0);
        }
    }
    return 0;
}

void TestSPMCQueue() {
    const int capacity = 1024;
    const int consumers = 4;
    const int total_items = 1000000;

    SPMCQueue<int> queue(capacity);

    // producer
    SPMCParam* producerParam = new SPMCParam{ &queue, total_items, nullptr };
    HANDLE producerHandle = (HANDLE)_beginthreadex(
        nullptr, 0, &SPMCProducerThread, producerParam, 0, nullptr);

    // consumers
    std::vector<std::vector<int>> consumerBuffers(consumers);
    std::vector<HANDLE> consumerHandles;
    for (int c = 0; c < consumers; ++c) {
        SPMCParam* param = new SPMCParam{ &queue, total_items, &consumerBuffers[c] };
        HANDLE h = (HANDLE)_beginthreadex(nullptr, 0, &SPMCConsumerThread, param, 0, nullptr);
        consumerHandles.push_back(h);
    }

    // join
    WaitForSingleObject(producerHandle, INFINITE);
    for (HANDLE h : consumerHandles) WaitForSingleObject(h, INFINITE);

    CloseHandle(producerHandle);
    for (HANDLE h : consumerHandles) CloseHandle(h);

    // °ËÁõ
    size_t sum = 0;
    for (auto& buf : consumerBuffers) sum += buf.size();

    if (sum == (size_t)total_items) {
        std::cout << "SPMC Queue Test Passed! " << total_items << " items consumed.\n";
    }
    else {
        std::cout << "SPMC Queue Test Failed! Got " << sum
            << ", expected " << total_items << "\n";
    }
}
