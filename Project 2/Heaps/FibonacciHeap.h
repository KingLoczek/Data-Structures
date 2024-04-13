#ifndef FIB_HEAP_H
#define FIB_HEAP_H

#include "PriorityQueue.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <limits>

template<typename T, typename P = int>
class FibonacciHeap : public PriorityQueue<T, P>  {
        struct Node {
            T value;
            P priority;
            bool marked;
            size_t degree;
            Node* next;
            Node* prev;
            Node* child;
            Node* parent;

            Node(P priority, T value) : 
                value(value), 
                priority(priority), 
                marked(false), 
                degree(0), 
                next(nullptr), 
                prev(nullptr), 
                child(nullptr), 
                parent(nullptr) {
            }
        };

        Node* mRoots; // head of the roots linked list
        Node* mMax;
        size_t mSize;

        void addRoot(Node* node) {
            node->parent = nullptr;

            if (!mRoots) {
                node->prev = node;
                node->next = node;
            } else {
                node->prev = mRoots->prev;
                mRoots->prev->next = node;
                mRoots->prev = node;
                node->next = mRoots;
            }

            mRoots = node;
        }

        void delRoot(Node* node) {
            Node* prev = node->prev;
            Node* next = node->next;

            if (node == prev) {
                mRoots = nullptr;
            } else {
                if (prev) prev->next = next;
                if (next) next->prev = prev;

                if (mRoots == node) { mRoots = next; }
            }
        }

        void link(Node* adopted, Node* parent) {
            delRoot(adopted);

            adopted->parent = parent;
            Node* child = parent->child;
            if (child) { // grow the family
                adopted->next = child;
                adopted->prev = child->prev;
                child->prev->next = adopted;
                child->prev = adopted;
            } else { // start the family
                adopted->next = adopted;
                adopted->prev = adopted;
            }
            parent->child = adopted;

            parent->degree++;
            adopted->marked = false;
        }

        size_t log2(size_t x) {
            // NLZ counting from "Hacker's Delight" by H. Warren
            int n = 64;
            int c = 32;
            size_t y;
            do {
                y = x >> c; if (y != 0) { n = n - c; x = y; }
                c = c >> 1;
            } while (c != 0);

            // log2
            return 63 - (n - x);
        }

        void consolidate() {
            size_t size = 2 * (log2(mSize) + 1);

            Node** nodes = new Node*[size];
            memset(nodes, 0, size * sizeof(Node*));

            Node* iter = mRoots;
            // break cycle
            iter->prev->next = nullptr;
            iter->prev = nullptr;
            while (iter) {
                Node* a = iter;
                Node* b;
                size_t deg = a->degree;

                iter = iter->next;

                while ((b = nodes[deg])) {
                    if (a->priority > b->priority) {
                        link(b, a);
                    } else {
                        link(a, b);
                        a = b;
                    }

                    nodes[deg] = nullptr;
                    deg++;
                }
                nodes[deg] = a;
            }

            mMax = nullptr;
            mRoots = nullptr;

            for (size_t i = 0; i < size; i++) {
                Node* n = nodes[i];

                if (n) {
                    addRoot(n);

                    if (!mMax || n->priority > mMax->priority) { mMax = n; }
                }
            }

            delete[] nodes;
        }

        Node* doFind(Node* start, const T& value) {
            if (!start) return nullptr;

            Node* iter = start;

            do {
                if (iter->value == value) {
                    return iter;
                }

                if (iter->child) {
                    doFind(iter->child, value);
                }

                iter = iter->next;
            } while (iter != start);

            return nullptr;
        }

        P maxPriority() {
            return std::numeric_limits<P>::max();
        }

        Node* cutMax() {
            Node* max = mMax;

            if (max->child) {
                Node* start = max->child;
                Node* child = start;

                do {
                    child->parent = nullptr;
                    child = child->next;
                    addRoot(child);
                } while (child != start);
            }

            delRoot(max);

            if (max->next == max) {
                mMax = nullptr;
                mRoots = nullptr;
            } else {
                mMax = max->next;
                consolidate();
            }
            mSize--;

            return max;
        }

        void doInsert(Node* node) {
            addRoot(node);

            if (!mMax || node->priority > mMax->priority) { mMax = node; }

            mSize++;
        }

        void cut(Node* orphan, Node* parent) {
            Node* next = orphan->next;
            Node* prev = orphan->prev;

            if (next == orphan) {
                parent->child = nullptr;
            } else {
                next->prev = prev;
                prev->next = next;

                if (parent->child == orphan) {
                    parent->child = next;
                }
            }
            parent->degree--;

            orphan->parent = nullptr;
            orphan->marked = false;
            addRoot(orphan);
        }

        void cascadingCut(Node* node) {
            Node* parent = node->parent;
            if (parent) {
                if (!node->marked) {
                    node->marked = true;
                } else {
                    cut(node, parent);
                    cascadingCut(parent);
                }
            }
        }


    public:
        FibonacciHeap() : mRoots(nullptr), mMax(nullptr), mSize(0) {
        }

        ~FibonacciHeap() {
            mMax = nullptr;
            mSize = 0;

            if (mRoots) {
                Node* head = mRoots;
                Node* tail = mRoots->prev;
                head->prev = nullptr;
                tail->next = nullptr;
                mRoots = nullptr;

                Node* iter;
                while ((iter = head)) {
                    head = head->next;
                    if (!head) tail = nullptr;

                    Node* child = iter->child;

                    if (child) {
                        Node* lastChild = child->prev;
                        child->prev = nullptr;
                        lastChild->next = nullptr;

                        if (tail) {
                            tail->next = child;
                            tail = lastChild;
                        } else {
                            head = child;
                            tail = lastChild;
                        }
                    }

                    delete iter;
                }
            }
        }

        void insert(P priority, T item) override {
            Node* node = new Node(priority, item);
            doInsert(node);
        }

        T extractMax() override {
            assert(mMax && "Heap must not be empty");

            Node* max = cutMax();

            T value = max->value;
            delete max;
            return value;
        }

        void decreaseKey(Node* node, P priority) {
            if (!node || node->priority == priority) return;
            assert(node->priority > priority && "new key must be less than current key");

            increaseKey(node, maxPriority());
            cutMax();
            node->priority = priority;
            doInsert(node);
        }

        void increaseKey(Node* node, P priority) {
            if (!node || node->priority == priority) return;
            assert(node->priority < priority && "new key must be greater than current key");

            node->priority = priority;
            Node* parent = node->parent;

            if (parent && node->value < parent->value) {
                cut(node, parent);
                cascadingCut(parent);
            }

            if (node->value > mMax->value) {
                mMax = node;
            }
        }

        void modifyKey(Node* node, P priority) {
            if (!node || node->priority == priority) return;

            if (node->priority > priority) {
                decreaseKey(node, priority);
            } else {
                increaseKey(node, priority);
            }
        }

        Node* find(T value) {
            Node* root = mRoots;
            do {
                if (root->value == value) {
                    return root;
                } else if (value < root->value) {
                    Node* needle;
                    if (root->child && (needle = doFind(root->child, value))) {
                        return needle;
                    }
                }

                root = root->next;
            } while (root != mRoots);

            return nullptr;
        }

        void modifyKey(T data, P priority) override {
            Node* node = find(data);
            modifyKey(node, priority);
        }

        T findMax() override {
            return mMax->value;
        }

        size_t size() {
            return mSize;
        }
};

#endif
