#include <iostream>
#include <cstdlib>
#include <ctime>
#include "BinaryHeap.h"

int main() {
    srand(time(NULL));
    // Creating a binary heap
    BinaryHeap<Node> binaryHeap(1);

    // Adding elements to the binary heap
    std::cout << "Adding elements to the binary heap:\n";
    for (int i = 0; i < 10; ++i) {
        Node node;
        node.data = i;
        node.priority = rand() % 100; // Random priority in range [0, 99]
        binaryHeap.insert(node);
        std::cout << "Added Node: " << node.data << " (Priority: " << node.priority << ")\n";
    }

    // Displaying the maximum element
    std::cout << "Max element: " << binaryHeap.findMax().data << " (" << binaryHeap.findMax().priority << ")\n";

    // Modifying the priority of an element
    binaryHeap.modifyKey(2, 12);

    // Displaying the maximum element after modification
    std::cout << "Max element after modification: " << binaryHeap.findMax().data << " (" << binaryHeap.findMax().priority << ")\n";

    // Extracting the maximum element
    Node maxNode = binaryHeap.extractMax();
    std::cout << "Extracted max element: " << maxNode.data << " (" << maxNode.priority << ")\n";

    // Displaying the size of the heap
    std::cout << "Heap size: " << binaryHeap.returnSize() << "\n";

    // Displaying the maximum element after modification
    std::cout << "Max element after modification: " << binaryHeap.findMax().data << " (" << binaryHeap.findMax().priority << ")\n";

    // Popping elements from the binary heap
    std::cout << "\nPopping elements from the binary heap:\n";
    while (binaryHeap.returnSize() > 0) {
        Node maxNode = binaryHeap.extractMax();
        std::cout << "Extracted Node: " << maxNode.data << " (Priority: " << maxNode.priority << ")\n";
    }

    return 0;
}