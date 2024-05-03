#ifndef PAIRING_HEAP_H
#define PAIRING_HEAP_H

#include "PriorityQueue.h"
#include <cstdio>
#include <deque>
#include <stdexcept>

template<typename T, typename P>
class PairingHeap : public PriorityQueue<T,P>{
private:
    struct Node
    {
        T value;
        P priority;
        Node* leftChild;
        Node* nextSibling;

        Node(P priority, T value): 
        priority(priority), 
        value(value), 
        leftChild(nullptr),
        nextSibling(nullptr){}
    };
    
    Node* root;
    size_t lenght; 

    Node* meld(Node* a, Node* b){
        // If any of the two-nodes is null 
        // the return the not null node
        if (!a) return b;
        if (!b) return a;
        
        // To maintain the max heap condition compare    
        // the nodes and node with max value become  
        // parent of the other node
        if(a->priority > b->priority){
            b->nextSibling = a->leftChild;
            a->leftChild = b;
            return a;
        }else{
            a->nextSibling = b->leftChild;
            b->leftChild = a;
            return b;
        }
    }

    // https://www.cise.ufl.edu/~sahni/dsaaj/enrich/c13/pairing.htm
    Node* meldPairs(Node* start){
        if(!start || !start->nextSibling) return start;

        // Two pass: 
        // 1. left -> right meld pairs
        // 2. right -> left meld pairs
        std::deque<Node*> queue;

        Node* node = start;
        while (node) {
            queue.push_back(node);
            node = node->nextSibling;
        }

        while(queue.size() >= 2) {
            Node* left = queue.front();
            queue.pop_front();
            Node* right = queue.front();
            queue.pop_front();
            queue.push_back(meld(left, right));
        }
        return queue.front();
    }
    Node* findParent(Node* start, Node* child){
        if(!start || !child) return nullptr;

        if(start->leftChild == child || start->nextSibling == child)
            return start;
        
        Node* parent = findParent(start->leftChild, child);
        if(!parent)
            parent = findParent(start->nextSibling,child);
        return parent;
    }

    //Cut child from parent
    void cut(Node* node){
        if(!node->nextSibling) return;

        Node* parent = findParent(root, node);
        if(parent){
            if(parent->leftChild == node)
                parent->leftChild = node->nextSibling;
            else
                parent->nextSibling = node->nextSibling;
        }
        node->nextSibling = nullptr;
    }
    //Find element in heap
    Node* find(Node* start, T value){
        if(!start) return nullptr;

        if(start->value == value) return start;

        Node* found = find(start->leftChild,value);
        if(!found)
            found = find(start->nextSibling, value);
        return found;
    }
    
public:
    PairingHeap() : root(nullptr), lenght(0) {}

    void insert(P priority, T value) override{
        Node* newNode = new Node(priority, value);
        root = meld(root, newNode);
        lenght++;
    }

    T extractMax() override{
        if(!root) throw std::logic_error("Heap is empty");

        T maxValue = root->value;
        Node* oldRoot = root;
        root = meldPairs(root->leftChild);
        delete oldRoot;
        lenght--;

        return maxValue;
    }

    T findMax() override{
        if(!root) throw std::logic_error("Heap is empty");

        return root->value;
    }

    void modifyKey(T value, P newPriority) override{
        Node* target = find(root, value);
        if(!target) throw std::logic_error("Element not found");

        target->priority = newPriority;
        if(target != root){
            cut(target);
            root = meld(root, target);
        }
    }

    size_t size() override{
        return lenght;
    }
};

#endif