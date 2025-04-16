#include <iostream>
#include "CHeap.h" // Replace this with the actual name of your header if separated

using namespace std; 

void CHeapTest()
{
    MaxHeap<int> maxHeap;
    MinHeap<int> minHeap;

    // Test values
    int values[] = { 32, 16, 64, 48, 5, 3, 27, 99, 45, 92, 10, 1, 7, 68, 23 };

    cout << "Pushing into MaxHeap:\n";
    for (int val : values) {
        maxHeap.push(val);
    }
	for (MaxHeap<int>::iterator it = maxHeap.begin(); it != maxHeap.end(); ++it) {
		cout << *it << " ";
	}

    cout << "\n"; 

	maxHeap.sort(); // Sort the heap in ascending order 
	cout << "\nSorted MaxHeap:\n";
    for (MaxHeap<int>::iterator it = maxHeap.begin(); it != maxHeap.end(); ++it) {
        cout << *it << " ";
    }

    cout << "\n";

    cout << "\nMaxHeap Pop Sequence:\n";
    while (!maxHeap.empty()) {
        cout << maxHeap.pop() << " ";
    }
    cout << "\n";

    cout << "\nPushing into MinHeap:\n";
    for (int val : values) {
        cout << val << " ";
        minHeap.push(val);
    }

    cout << "\nMinHeap Pop Sequence:\n";
    while (!minHeap.empty()) {
        cout << minHeap.pop() << " ";
    }
    cout << "\n";
}