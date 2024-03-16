#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <algorithm>
#include <cstddef>
#include <memory>
#include <assert.h>
#include <iostream>

#include "IList.h"

template<typename T>
class DoublyLinkedList : public IList<T> {
    private:
        struct Node {
            T item;
            Node* next;
            Node* prev;

            Node(T&& item, Node* next, Node* prev) : item(std::move(item)), next(next), prev(prev) {
            }
        };

        Node* mHead;
        Node* mTail;
        size_t mLength;

        Node* findNode(size_t index) {
           assert(index < mLength && "Out of bounds");

           if (index < mLength / 2) {
               // index is in the lower half of the list, so it's less work to get to it from the head
               Node* needle = mHead;
               for (size_t i = 0; i < index; i++) { // go index times forwards
                   needle = needle->next;
               }
               return needle;
           } else {
               // index is in the upper half (or the middle, which does not really matter), so go from the tail
               Node* needle = mTail;
               for (size_t i = mLength - 1; i > index; i--) { // go index times backwards
                   needle = needle->prev;
               }
               return needle;
           }
        }

    public:
        DoublyLinkedList() : mHead(nullptr), mTail(nullptr), mLength(0) {
        }

        virtual ~DoublyLinkedList() {
            Node* next = mHead;

            while (next) {
                Node* canned = next;
                next = next->next;
                delete canned;
            }

            mHead = nullptr;
            mTail = nullptr;
        }

        virtual T& get(size_t index) override {
            return findNode(index)->item;
        }
        
        virtual void add(T element) override {
            Node* node = new Node(std::move(element), nullptr, mTail);
            if (!mHead) mHead = node; // no head, so this element must be the first, init head too

            if (mTail) mTail->next = node;
            mTail = node;
            mLength += 1;
        }

        virtual void add(size_t index, T element) override {
            Node* existing = findNode(index);
            Node* prev = existing->prev;
            Node* node = new Node(std::move(element), existing, prev);
            prev->next = node;
            existing->prev = node;
            mLength += 1;
        }

        virtual void push(T element) override {
            Node* node = new Node(std::move(element), mHead, nullptr);
            if (!mTail) mTail = node; // no tail, so this element must be the first, init tail too

            if (mHead) mHead->prev = node;
            mHead = node;
            mLength += 1;
        }

        virtual T remove() override {
            assert(mLength > 0 && "Cannot remove from empty list!");
            Node* evicted = mTail;
            mTail = evicted->prev;
            if (!mTail) mHead = nullptr; // last one, reset head also
            else mTail->next = nullptr;
            T item(std::move(evicted->item));
            mLength -= 1;
            delete evicted;
            return item;
        }

        virtual T remove(size_t index) override {
            if (index == 0) return pop();
            if (index == mLength - 1) return remove();

            Node* evicted = findNode(index);
            Node* next = evicted->next;
            Node* prev = evicted->prev;
            if (next) next->prev = prev;
            if (prev) prev->next = next;
            T item(std::move(evicted->item));
            mLength -= 1;
            delete evicted;
            return item;
        }

        virtual T pop() override {
            assert(mLength > 0 && "Cannot pop from empty list!");
            Node* evicted = mHead;
            mHead = evicted->next;
            if (!mHead) mTail = nullptr; // last one, reset tail also
            else mHead->prev = nullptr;
            T item(std::move(evicted->item));
            mLength -= 1;
            delete evicted;
            return item;
        }

        virtual bool contains(const T& element) override {
            Node* needle = mHead;
            while (needle) {
                if (element == needle->item) return true;
                needle = needle->next;
            }
            return false;
        }

        virtual size_t size() override {
            return mLength;
        }
};

#endif
