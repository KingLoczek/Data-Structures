#include "DoublyLinkedList.h"
#include "SinglyLinkedHead.h"
#include "SinglyLinkedList.h"
#include "arraylist.h"
#include <chrono>
#include <cstddef>
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

InstantT start() {
    return std::chrono::high_resolution_clock::now();
}

TimeT end(const InstantT& start) {
    auto end = std::chrono::high_resolution_clock::now();
    TimeT duration = std::chrono::duration_cast<TimeT>(end - start);
    return duration;
}

int* makeDataSet(size_t N, int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(0, 0xFFFFFFF);

    int* items = new int[N];
    for (size_t i = 0; i < N; i++) {
        items[i] = dist(gen);
    }
    
    return items;
}

int* makeDataSet(size_t N) {
    std::mt19937 gen;
    std::uniform_int_distribution<> dist(0, 0xFFFFFFF);

    int* items = new int[N];
    for (size_t i = 0; i < N; i++) {
        items[i] = dist(gen);
    }
    
    return items;
}

template<typename T>
T* makeLists(size_t len, int N) {
    T* lists = new T[len];

    int* items = makeDataSet(static_cast<size_t>(N), N);
    for (size_t j = 0; j < len; j++) {
        lists[j].addAll(static_cast<size_t>(N), items);
    }

    delete[] items;
    return lists;
}

template<typename T>
T* fillLists(size_t nData, int* data, size_t len) {
    T* lists = new T[len];

    for (size_t i = 0; i < len; i++) {
        lists[i].addAll(nData, data);
    }

    return lists;
}

template<typename T>
void benchAddBegin(size_t N, size_t m, int* data, TimeT& timer) {
    T* lists = fillLists<T>(N, data, m);

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
void benchAddEnd(int N, size_t m, int* data, TimeT& timer) {
    T* lists = fillLists<T>(N, data, m);
    auto begin = start();
    for (size_t i = 0; i < m; i++) {
        lists[i].add(69420);
    }
    timer += end(begin);

    doNotOptimize(lists);
    delete[] lists;
}

template<typename T>
void benchAdd(int N, size_t m, int* data, TimeT& timer) {
    T* lists = fillLists<T>(N, data, m);
    auto begin = start();
    for (size_t i = 0; i < m; i++) {
        lists[i].add(1337, N / 2);
    }
    timer += end(begin);

    doNotOptimize(lists);
    delete[] lists;
}

template<typename F>
void run(const char* tag, const char* name, int N, size_t reps, size_t m, F bench) {
    TimeT acc{0};

    for (size_t i = 0; i < reps; i++) {
        int* data = makeDataSet(static_cast<size_t>(N), N + i);
        bench(N, m, data, acc);
    }

    std::cout << tag << "," << name << "," << N << "," << m << "," << reps << "," << acc.count() << '\n';
}

template<typename F>
void warmup(const char* tag, const char* name, size_t N, size_t reps, size_t m, int* data, F bench) {
    TimeT acc{0};
    for (size_t i = 0; i < reps; i++) {
        bench(N, m, data, acc);
    }
}

template<typename F>
void run(const char* tag, const char* name, size_t N, size_t reps, size_t m, int* data, F bench) {
    TimeT acc{0};

    for (size_t i = 0; i < reps; i++) {
        bench(N, m, data, acc);
    }

    std::cout << tag << "," << name << "," << N << "," << m << "," << reps << "," << acc.count() << '\n';
}

template<typename F>
void runner(const char* tag, const char* name, const std::vector<int>& Ns, size_t reps, size_t m, F bench) {
    for (int n : Ns) {
        run(tag, name, n, reps, m, bench);
    }
}

int main(int argc, char **argv) {
    //std::vector<int> ns{ 4000, 8000, 10000, 16000, 20000, 40000, 60000, 100000 };
    std::vector<int> ns;
    for (int i = 1; i <= 20; i++) {
        int n = i * 5000;
        ns.push_back(n);
    }
    std::cout << "struct,bench,N,no_instances,reps,total_time_ns\n";

    const size_t WARMUPS = 2;
    const size_t RUNS = 4;
    const size_t STRUCTS = 100;
    const size_t EPOCHS = 1;
    const size_t A0 = 5000;
    const size_t A19 = 20 * A0;

    for (size_t N = A0; N <= A19; N += A0) {
        for (size_t i = 0; i < EPOCHS; i++) {
            int* set = makeDataSet(N);

            warmup(       "ArrayList", "AddBegin", N, WARMUPS, STRUCTS, set, benchAddBegin<ArrayList<int>>);
            run(          "ArrayList", "AddBegin", N,    RUNS, STRUCTS, set, benchAddBegin<ArrayList<int>>);
            warmup("SinglyLinkedHead", "AddBegin", N, WARMUPS, STRUCTS, set, benchAddBegin<SinglyLinkedHead<int>>);
            run(   "SinglyLinkedHead", "AddBegin", N,    RUNS, STRUCTS, set, benchAddBegin<SinglyLinkedHead<int>>);
            warmup("SinglyLinkedList", "AddBegin", N, WARMUPS, STRUCTS, set, benchAddBegin<SinglyLinkedList<int>>);
            run(   "SinglyLinkedList", "AddBegin", N,    RUNS, STRUCTS, set, benchAddBegin<SinglyLinkedList<int>>);
            warmup("DoublyLinkedList", "AddBegin", N, WARMUPS, STRUCTS, set, benchAddBegin<DoublyLinkedList<int>>);
            run(   "DoublyLinkedList", "AddBegin", N,    RUNS, STRUCTS, set, benchAddBegin<DoublyLinkedList<int>>);
        }
    }

    for (size_t N = A0; N <= A19; N += A0) {
        for (size_t i = 0; i < EPOCHS; i++) {
            int* set = makeDataSet(N);

            warmup(       "ArrayList", "AddEnd", N,  WARMUPS, STRUCTS, set, benchAddEnd<ArrayList<int>>);
            run(          "ArrayList", "AddEnd", N,     RUNS, STRUCTS, set, benchAddEnd<ArrayList<int>>);
            warmup("SinglyLinkedHead", "AddEnd", N,  WARMUPS, STRUCTS, set, benchAddEnd<SinglyLinkedHead<int>>);
            run(   "SinglyLinkedHead", "AddEnd", N,     RUNS, STRUCTS, set, benchAddEnd<SinglyLinkedHead<int>>);
            warmup("SinglyLinkedList", "AddEnd", N,  WARMUPS, STRUCTS, set, benchAddEnd<SinglyLinkedList<int>>);
            run(   "SinglyLinkedList", "AddEnd", N,     RUNS, STRUCTS, set, benchAddEnd<SinglyLinkedList<int>>);
            warmup("DoublyLinkedList", "AddEnd", N,  WARMUPS, STRUCTS, set, benchAddEnd<DoublyLinkedList<int>>);
            run(   "DoublyLinkedList", "AddEnd", N,     RUNS, STRUCTS, set, benchAddEnd<DoublyLinkedList<int>>);
        }
    }

    for (size_t N = A0; N <= A19; N += A0) {
        for (size_t i = 0; i < EPOCHS; i++) {
            int* set = makeDataSet(N);

            warmup(       "ArrayList", "Add", N,  WARMUPS, STRUCTS, set, benchAdd<ArrayList<int>>);
            run(          "ArrayList", "Add", N,     RUNS, STRUCTS, set, benchAdd<ArrayList<int>>);
            warmup("SinglyLinkedHead", "Add", N,  WARMUPS, STRUCTS, set, benchAdd<SinglyLinkedHead<int>>);
            run(   "SinglyLinkedHead", "Add", N,     RUNS, STRUCTS, set, benchAdd<SinglyLinkedHead<int>>);
            warmup("SinglyLinkedList", "Add", N,  WARMUPS, STRUCTS, set, benchAdd<SinglyLinkedList<int>>);
            run(   "SinglyLinkedList", "Add", N,     RUNS, STRUCTS, set, benchAdd<SinglyLinkedList<int>>);
            warmup("DoublyLinkedList", "Add", N,  WARMUPS, STRUCTS, set, benchAdd<DoublyLinkedList<int>>);
            run(   "DoublyLinkedList", "Add", N,     RUNS, STRUCTS, set, benchAdd<DoublyLinkedList<int>>);
        }
    }

    return 0;
}
