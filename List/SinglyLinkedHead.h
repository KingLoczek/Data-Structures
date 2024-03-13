#ifndef SINGLYLINKEDHEAD_H
#define SINGLYLINKEDHEAD_H
#include "IList.h"

template<typename T>
class SinglyLinkedHead : public IList<T> {
private:
    struct Node {
        T data;
        Node* next;

        Node(T data) : data(data), next(nullptr) {}
    };
    Node* head;
    size_t listSize;

public:
    SinglyLinkedHead() : head(nullptr), listSize(0) {}

    virtual ~SinglyLinkedHead() {
        Node* current = head;
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        listSize = 0;
    }

    virtual void add(T element) override {
        Node* newNode = new Node(element);
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        listSize++;
    }

    virtual void push(T element) override {
        Node* newNode = new Node(element);
        newNode->next = head;
        head = newNode;
        listSize++;
    }

    virtual void add(size_t index, T element) override {
        if (index > listSize) {
            std::cerr << "Index out of bounds." << std::endl;
            return;
        }
        if (index == 0) {
            push(element);
        } else if (index == listSize) {
            add(element);
        } else {
            Node* newNode = new Node(element);
            Node* current = head;
            for (size_t i = 0; i < index - 1; ++i) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
            listSize++;
        }
    }

    virtual T remove() override{
        if (!head) {
            std::cerr << "List is empty." << std::endl;
            exit(EXIT_FAILURE);
        }
        Node* current = head;
        Node* previous = nullptr;
        while (current->next) {
            previous = current;
            current = current->next;
        }
        T removedData = current->data;
        if (previous) {
            previous->next = nullptr;
        } else {
            head = nullptr;
        }
        delete current;
        listSize--;
        return removedData;
    }

    virtual T pop() override{
        if (!head) {
            std::cerr << "List is empty." << std::endl;
            exit(EXIT_FAILURE);
        }
        T removedData = head->data;
        Node* temp = head;
        head = head->next;
        delete temp;
        listSize--;
        return removedData;
    }

    virtual T remove(size_t index) override{
        if (index >= listSize) {
            std::cerr << "Index out of bounds." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (index == 0) {
            return pop();
        } else {
            Node* current = head;
            Node* previous = nullptr;
            for (size_t i = 0; i < index; ++i) {
                previous = current;
                current = current->next;
            }
            T removedData = current->data;
            previous->next = current->next;
            delete current;
            listSize--;
            return removedData;
        }
    }

    virtual bool contains(const T& element) override{
        Node* current = head;
        while (current) {
            if (current->data == element) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    
    virtual T& get(size_t index) override{
        if (index >= listSize) {
            std::cerr << "Index out of bounds." << std::endl;
            exit(EXIT_FAILURE);
        }
        Node* current = head;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    virtual size_t size() override{
        return listSize;
    }
};

#endif