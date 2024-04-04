#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include "IList.h"
#include <memory>

template<typename T>
class SinglyLinkedList : public IList<T> {
private:

    struct Node {
        T data;
        Node* next;

        Node(T data) : data(data), next(nullptr) {}
    };
    Node* head;
    Node* tail;
    size_t listSize;

public:
    SinglyLinkedList() : head(nullptr), tail(nullptr), listSize(0) {}

    ~SinglyLinkedList() {
        Node* current = head;
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        listSize = 0;
    }

    void add(T element) override{
        Node* newNode = new Node(element);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        listSize++;
    }

    void push(T element) override{
        Node* newNode = new Node(element);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        listSize++;
    }

    void add(size_t index, T element) override{
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

    virtual void addAll(size_t len, T* arr) override {
        for (size_t i = 0; i < len; i++) {
            push(std::move(arr[i]));
        }
    }

    T remove() override{
        if (!head) {
            std::cerr << "List is empty." << std::endl;
            exit(EXIT_FAILURE);
        }
        T removedData = tail->data;
        if (head == tail) {
            delete head;
            head = nullptr;
            tail = nullptr;
        } else {
            Node* current = head;
            while (current->next != tail) {
                current = current->next;
            }
            delete tail;
            tail = current;
            tail->next = nullptr;
        }
        listSize--;
        return removedData;
    }

    T pop() override{
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

    T remove(size_t index) override{
        if (index >= listSize) {
            std::cerr << "Index out of bounds." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (index == 0) {
            return pop();
        } else if (index == listSize - 1) {
            return remove();
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

    bool contains(const T& element) override{
        Node* current = head;
        while (current) {
            if (current->data == element) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    T& get(size_t index) override{
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

    size_t size() override{
        return listSize;
    }
};

#endif
