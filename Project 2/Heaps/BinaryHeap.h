#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H
#include <utility>
#include <cassert>

#include "PriorityQueue.h"

template<typename T>
class BinaryHeap:public PriorityQueue<T>{
private:
    T* heapArray; // Array to store heap elements
    int capacity; // Maximum capacity of the heap
    int size;     // Current size of the heap

    void heapifyUp(int index){
        int parent = (index - 1) / 2;
        while (index > 0 && heapArray[parent].priority < heapArray[index].priority)
        {
            std::swap(heapArray[index], heapArray[parent]);
            index = parent;
            parent = (index - 1) / 2;
        }
    };

    void heapifyDown(int index){
        while (true){    
            int leftChild = 2*index+1;
            int rightChild = 2*index+2;
            int largest = index;

            if (leftChild < size && heapArray[leftChild].priority > heapArray[largest].priority){
                largest = leftChild;
            }
            if (rightChild < size && heapArray[rightChild].priority > heapArray[largest].priority){
                largest = rightChild;
            }
            if(largest != index){
                std::swap(heapArray[index], heapArray[largest]);
                index = largest;
            }else{
                break;
            }
        }
    };
public:
    BinaryHeap(int capacity){
        this->capacity = capacity;
        size = 0;
        heapArray = new T[capacity];
    };

    ~BinaryHeap(){
        delete[] heapArray;
    };

    void insert(T element) override{
        if(size == capacity){
            capacity *=2;
            T* newArray = new T[capacity];
            for (size_t i = 0; i < size; i++)
                newArray[i] = heapArray[i];
            delete[] heapArray;
            heapArray = newArray;
        }
        heapArray[size] = element; // Add data to the end of queue
        heapifyUp(size); // Repair heap
        size++; // Increase size of queue
    };

    T extractMax() override{
        if (size == 0) {
            std::cout << "Heap is empty, cannot extract max.\n";
            return T();
        }

        T maxNode = heapArray[0];
        heapArray[0] = heapArray[size - 1];
        size--;
        heapifyDown(0);
        return maxNode;
    };

    T findMax() override{
        if (size == 0) {
            std::cout << "Heap is empty, cannot find max.\n";
            return T();
        }

        return heapArray[0];
    };

    void modifyKey(int data, int newPriority) override {
        for (int i = 0; i < size; ++i) {
            if (heapArray[i].data == data) {
                if (newPriority > heapArray[i].priority) {
                    heapArray[i].priority = newPriority;
                    heapifyUp(i);
                } else {
                    heapArray[i].priority = newPriority;
                    heapifyDown(i);
                }
                return;
            }
        }
        std::cout << "Element not found in the heap.\n";
    };

    int returnSize() override{
        return size;
    };
};

#endif