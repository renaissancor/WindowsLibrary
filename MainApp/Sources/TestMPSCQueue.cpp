#include "pch.h"
#include "MPSCQueue.h"

struct MPSCParam {
    MPSCQueue<int>* queue;
    int producer_id;
    int items_per_producer;
    std::vector<int>* consumed_items;
};

static unsigned int __stdcall MPSCConsumerThread(void* arg) noexcept {
    MPSCParam* param = static_cast<MPSCParam*>(arg);
    MPSCQueue<int>* queue = param->queue;
    int total_to_consume = param->producer_id * param->items_per_producer;
    int received = 0;

    while (received < total_to_consume) {
        int item;
        if (queue->Pop(item)) {
            param->consumed_items->push_back(item);
            ++received;
        }
        else {
            Sleep(0);
        }
    }
    return 0;
}

static unsigned int __stdcall MPSCProducerThread(void* arg) noexcept {
    MPSCParam* param = static_cast<MPSCParam*>(arg);
    MPSCQueue<int>* queue = param->queue;

    int base = param->producer_id * param->items_per_producer;
    for (int i = 0; i < param->items_per_producer; ++i) {
        while (!queue->Push(base + i)) {
            Sleep(0);
        }
    }
    return 0;
}

void TestMPSCQueue() {
    const int capacity = 1024;
    const int producers = 4;
    const int items_per_producer = 250000;
    const int total_items = producers * items_per_producer;

    MPSCQueue<int> queue(capacity);
    std::vector<int> consumed_items;
    consumed_items.reserve(total_items);

    // Consumer Single 
    MPSCParam* consumerParam = new MPSCParam{ &queue, producers, items_per_producer, &consumed_items };
    HANDLE consumerHandle = (HANDLE)_beginthreadex(
        nullptr, 0, &MPSCConsumerThread, consumerParam, 0, nullptr);

    // Producer Multiple
    std::vector<HANDLE> producerHandles;
    for (int p = 0; p < producers; ++p) {
        MPSCParam* param = new MPSCParam{ &queue, p, items_per_producer, &consumed_items };
        HANDLE h = (HANDLE)_beginthreadex(nullptr, 0, &MPSCProducerThread, param, 0, nullptr);
        producerHandles.push_back(h);
    }

    if (!consumerHandle || producerHandles.size() != producers) {
        std::cerr << "Error: Unable to create threads.\n";
        return;
    }
    WaitForSingleObject(consumerHandle, INFINITE);
    for (HANDLE h : producerHandles) WaitForSingleObject(h, INFINITE);

    CloseHandle(consumerHandle);
    for (HANDLE h : producerHandles) CloseHandle(h);

    // Verify all items are consumed 
    if (consumed_items.size() == total_items) {
        std::cout << "MPSC Queue Test Passed! " << total_items << " items processed.\n";
    }
    else {
        std::cout << "MPSC Queue Test Failed! Got " << consumed_items.size()
            << ", expected " << total_items << "\n";
    }
}
