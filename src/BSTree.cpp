#include <iostream>
#include "BSTree.h" // your header file

void BSTreeTest()
{
    BSTree<int, float> tree;

    std::cout << "Inserting elements...\n";
    tree.insert(make_pair(50, 1.1f));
    tree.insert(make_pair(30, 2.2f));
    tree.insert(make_pair(70, 3.3f));
    tree.insert(make_pair(20, 4.4f));
    tree.insert(make_pair(40, 5.5f));
    tree.insert(make_pair(60, 6.6f));
    tree.insert(make_pair(80, 7.7f));
    tree.insert(make_pair(10, 8.8f));
    tree.insert(make_pair(25, 9.9f));
    tree.insert(make_pair(35, 10.10f));
    tree.insert(make_pair(45, 11.11f));
    tree.insert(make_pair(55, 12.12f));
    tree.insert(make_pair(65, 13.13f));
    tree.insert(make_pair(75, 14.14f));
    tree.insert(make_pair(85, 15.15f));


    std::cout << "\nTree Size: " << tree.size() << "\n";

    typename BSTree<int, float>::iterator it = tree.begin();

    std::cout << "\nIn-order Traversal increment:\n";
    for (; it != tree.end(); ++it)
    {
        std::cout << "Key: " << (*it).key << ", Value: " << (*it).val << "\n";
    }

    std::cout << "\nIn-order Traversal decrement:\n";
    for (--it; it != tree.begin(); --it)
    {
        std::cout << "Key: " << (*it).key << ", Value: " << (*it).val << "\n";
    }


    std::cout << "\nFinding keys:\n";
    int keysToFind[] = { 40, 100, 80 };
    for (int key : keysToFind)
    {
        auto it = tree.find(key);
        if (it != tree.end())
            std::cout << "Found key " << key << " with value: " << (*it).val << "\n";
        else
            std::cout << "Key " << key << " not found in the tree.\n";
    }

    std::cout << "\nClearing tree...\n";
    tree.clear();
    std::cout << "Tree Size after clear: " << tree.size() << "\n";
}

