#include <iostream>
#include "PQHeap.h" // Replace this with the actual name of your header if separated

using namespace std; 

void PQHeapTest()
{
    MaxPQHeap<int> maxPQHeap;
    MinPQHeap<int> minPQHeap;

    // Test values
    int values[] = { 32, 16, 64, 48, 5, 3, 27, 99, 45, 92, 10, 1, 7, 68, 23 };

    cout << "Pushing into MaxPQHeap:\n";
    for (int val : values) {
        cout << val << " ";
        maxPQHeap.push(val);
    }
    cout << "\n";

    cout << "MaxPQHeap structure\n";
	for (MaxPQHeap<int>::iterator it = maxPQHeap.begin(); it != maxPQHeap.end(); ++it) {
		cout << *it << " ";
	}
    cout << "\n"; 

	maxPQHeap.sort(); // Sort the heap in ascending order 
	cout << "\nSorted MaxPQHeap:\n";
    for (MaxPQHeap<int>::iterator it = maxPQHeap.begin(); it != maxPQHeap.end(); ++it) {
        cout << *it << " ";
    }

    cout << "\n";

    cout << "\nMaxPQHeap Pop Sequence:\n";
    while (!maxPQHeap.empty()) {
        cout << maxPQHeap.pop() << " ";
    }
    cout << "\n";

    cout << "\nPushing into MinPQHeap:\n";
    for (int val : values) {
        cout << val << " ";
        minPQHeap.push(val);
    }

    cout << "\nMinPQHeap Pop Sequence:\n";
    while (!minPQHeap.empty()) {
        cout << minPQHeap.pop() << " ";
    }
    cout << "\n";
}