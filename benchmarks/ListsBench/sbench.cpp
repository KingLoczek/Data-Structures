#include "DoublyLinkedList.h"
#include "SinglyLinkedHead.h"
#include "SinglyLinkedList.h"
#include "arraylist.h"
#include <chrono>
#include <ostream>
#include <ratio>
#include <vector>
#include <iostream>
#include <random>

#define DO_REPS

// Prevent compiler from optimizing away a variable, that is given as an argument to this function
// Uses Google Benchmark assembly magic
template<typename T>
void doNotOptimize(const T& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

using ItemT = int;
using TimeT = std::chrono::nanoseconds;
using InstantT = std::chrono::high_resolution_clock::time_point;

inline InstantT start() {
    return std::chrono::high_resolution_clock::now();
}

inline TimeT end(const InstantT& start) {
    auto end = std::chrono::high_resolution_clock::now();
    TimeT duration = std::chrono::duration_cast<TimeT>(end - start);
    return duration;
}


template<typename T>
T* makeLinkedLists(size_t len, int N) {
    T* lists = new T[len];
    std::mt19937 gen;
    std::uniform_int_distribution<> dist(0, 0xFFFFFFF);

    for (int i = 0; i < N; i++) {
        int item = dist(gen);

        for (size_t j = 0; j < len; j++) {
            lists[j].push(item);
        }
    }
}

template<typename T>
T* makeArrayLists(size_t len, int N) {
    T* lists = new T[len];
    std::mt19937 gen;
    std::uniform_int_distribution<> dist(0, 0xFFFFFFF);

    for (int i = 0; i < N; i++) {
        int item = dist(gen);

        for (size_t j = 0; j < len; j++) {
            lists[j].add(item);
        }
    }
}

template<typename T>
T* makeLists(size_t len, int N) {
    T* lists = new T[len];
    std::mt19937 gen;
    std::uniform_int_distribution<> dist(0, 0xFFFFFFF);

    int* items = new int[N];
    for (int i = 0; i < N; i++) {
        items[i] = dist(gen);
    }

    for (size_t j = 0; j < len; j++) {
        lists[j].addAll(static_cast<size_t>(N), items);
    }

    delete[] items;
    return lists;
}

template<typename T>
void benchAddBegin(int N, size_t m, TimeT& timer) {
    T* lists = makeLists<T>(m, N);
    auto begin = start();
    for (size_t i = 0; i < m; i++) {
        lists[i].push(2137);
    }
    timer += end(begin);

    // Fake lists use, to stop the compiler from figuring out that, really, the list is useless
    doNotOptimize(lists);
    delete[] lists;
}

template<typename T>
void benchAddEnd(int N, size_t m, TimeT& timer) {
    T* lists = makeLists<T>(m, N);
    auto begin = start();
    for (size_t i = 0; i < m; i++) {
        lists[i].add(69420);
    }
    timer += end(begin);

    doNotOptimize(lists);
    delete[] lists;
}

template<typename T>
void benchAdd(int N, size_t m, TimeT& timer) {
    T* lists = makeLists<T>(m, N);
    auto begin = start();
    for (size_t i = 0; i < m; i++) {
        lists[i].add(1337, N / 2);
    }
    timer += end(begin);

    doNotOptimize(lists);
    delete[] lists;
}

template<typename F>
void runner(const char* tag, const char* name, const std::vector<int>& Ns, size_t reps, size_t m, F bench) {
    for (int n : Ns) {
        TimeT acc{0};

        for (size_t i = 0; i < reps; i++) {
            bench(n, m, acc);
        }

        std::cout << tag << "," << name << "," << n << "," << m << "," << reps << "," << acc.count() << '\n';
    }
}

int main(int argc, char **argv) {
    std::vector<int> ns{ 4000, 8000, 10000, 16000, 20000, 40000, 60000, 100000 };
    std::cout << "struct,bench,N,no_instances,reps,total_time_ns\n";

    if (true) {
        runner(       "ArrayList", "AddBegin", ns, 10, 100, benchAddBegin<ArrayList<ItemT>>);
        runner("SinglyLinkedHead", "AddBegin", ns, 10, 100, benchAddBegin<SinglyLinkedHead<ItemT>>);
        runner("SinglyLinkedList", "AddBegin", ns, 10, 100, benchAddBegin<SinglyLinkedList<ItemT>>);
        runner("DoublyLinkedList", "AddBegin", ns, 10, 100, benchAddBegin<DoublyLinkedList<ItemT>>);
        std::cout << std::flush;
    }

    if (true) {
        runner(       "ArrayList", "AddEnd", ns, 10, 100, benchAddEnd<ArrayList<ItemT>>);
        runner("SinglyLinkedHead", "AddEnd", ns, 10, 100, benchAddEnd<SinglyLinkedHead<ItemT>>);
        runner("SinglyLinkedList", "AddEnd", ns, 10, 100, benchAddEnd<SinglyLinkedList<ItemT>>);
        runner("DoublyLinkedList", "AddEnd", ns, 10, 100, benchAddEnd<DoublyLinkedList<ItemT>>);
        std::cout << std::flush;
    }

    if (true) {
        runner(       "ArrayList", "Add", ns, 10, 100, benchAdd<ArrayList<ItemT>>);
        runner("SinglyLinkedHead", "Add", ns, 10, 100, benchAdd<SinglyLinkedHead<ItemT>>);
        runner("SinglyLinkedList", "Add", ns, 10, 100, benchAdd<SinglyLinkedList<ItemT>>);
        runner("DoublyLinkedList", "Add", ns, 10, 100, benchAdd<DoublyLinkedList<ItemT>>);
        std::cout << std::flush;
    }

    return 0;
}
