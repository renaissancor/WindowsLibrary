#include <iostream>
#include "CHeap.h" // Replace this with the actual name of your header if separated

int main() {
    MaxHeap<int> maxHeap;
    MinHeap<int> minHeap;

    // Test values
    int values[] = { 5, 3, 10, 1, 7 };

    std::cout << "Pushing into MaxHeap:\n";
    for (int val : values) {
        std::cout << "Push: " << val << "\n";
        maxHeap.push(val);
    }

    std::cout << "\nMaxHeap Pop Sequence:\n";
    while (!maxHeap.empty()) {
        std::cout << maxHeap.pop() << " ";
    }
    std::cout << "\n";

    std::cout << "\nPushing into MinHeap:\n";
    for (int val : values) {
        std::cout << "Push: " << val << "\n";
        minHeap.push(val);
    }

    std::cout << "\nMinHeap Pop Sequence:\n";
    while (!minHeap.empty()) {
        std::cout << minHeap.pop() << " ";
    }
    std::cout << "\n";

    return 0;
}
