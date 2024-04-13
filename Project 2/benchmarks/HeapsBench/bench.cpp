#include "BinaryHeap.h"
#include "FibonacciHeap.h"

#include <chrono>
#include <ostream>
#include <random>
#include <vector>

// Prevent compiler from optimizing away a variable, that is given as an argument to this function
// Uses Google Benchmark assembly magic
template<typename T>
void doNotOptimize(const T& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

struct Dummy {
    int data;

    bool operator==(const Dummy& other) {
        return data == other.data;
    }

    bool operator<(const Dummy& other) {
        return data < other.data;
    }

    bool operator>(const Dummy& other) {
        return data > other.data;
    }
};

const size_t MAX_SIZE = 10000000;

#define BENCH_IMPL(op) do { \
    auto begin = std::chrono::high_resolution_clock::now(); \
    op; \
    auto end = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin); \
    return duration.count(); \
} while(0)

template<typename H>
struct BenchInsert {
    long operator()(H* heap) {
        int benchP = (MAX_SIZE * 10) + 1;
        BENCH_IMPL(heap->insert(benchP, { 8086 }));
    }
};

template<typename H>
struct BenchExtract {
    long operator()(H* heap) {
        BENCH_IMPL(heap->extractMax());
    }
};

template<typename H>
struct BenchDecreaseKey {
    long operator()(H* heap) {
        auto node = heap->find(Dummy { 8086 });
        assert(node && "bench is brokey");
        BENCH_IMPL(heap->modifyKey(node, -1));
    }
};

template<typename H>
struct BenchIncreaseKey {
    long operator()(H* heap) {
        auto node = heap->find(Dummy { 8086 });
        assert(node && "bench is brokey");
        BENCH_IMPL(heap->modifyKey(node, MAX_SIZE * 10 + 1));
    }
};

#define BENCH_N_IMPL(op, iters) do { \
    auto begin = std::chrono::high_resolution_clock::now(); \
    for (size_t i = 0; i < iters; i++) { op; } \
    auto end = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin); \
    return duration.count(); \
} while(0)

template<typename H>
struct BenchFindMax {
    long operator()(H* heaps, size_t size)  {
        BENCH_N_IMPL(doNotOptimize(heaps[i].findMax()), size);
    }
};

template<typename H>
struct BenchReturnSize {
    long operator()(H* heaps, size_t size)  {
        BENCH_N_IMPL(doNotOptimize(heaps[i].size()), size);
    }
};

template<typename H, template<typename> typename A>
long bench(size_t N, int seed) {
    A<H> action;
    H heap;// = new BinaryHeap<Dummy>;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(0, MAX_SIZE * 10);
    std::uniform_int_distribution<> vDist(0, 4095);

    for (size_t n = 0; n < N - 1; n++) {
        int p = dist(gen);
        int v = vDist(gen);
        heap.insert( p, { v } );
    }
    int p = dist(gen);
    heap.insert(p, { 8086 });

    assert(heap.size() == N);
    return action(&heap);
}

template<typename H, template<typename> typename A>
long benchMulti(size_t N, int seed, size_t size, size_t batches) {
    A<H> action;
    H* heaps = new H[size * batches];// = new BinaryHeap<Dummy>;

    std::mt19937 gen;
    std::uniform_int_distribution<> dist(0, MAX_SIZE * 10);

    for (size_t n = 0; n < N; n++) {
        for (size_t i = 0; i < batches; i++) {
            int p = dist(gen);
            int v = 8086;

            for (size_t j = 0; j < size; j++) {
                heaps[i * size + j].insert( p, { v } );
            }
        }
    }

    assert(heaps[0].size() == N);
    long time = action(heaps, size*batches);
    delete[] heaps;
    return time;
}

template<template<typename> typename T>
void run(size_t N, size_t runs, const char* method) {
    long times[2] = { 0L, 0L };

    for (size_t i = 0; i < runs; i++) {
        std::mt19937 gen;
        std::uniform_int_distribution<> dist;

        int seed = dist(gen);
        times[0] += bench<BinaryHeap<Dummy>, T>(N, seed);
        times[1] += bench<FibonacciHeap<Dummy>, T>(N, seed);
    }

    std::cout << "BinHeap," << method << "," << N << ",1," << runs << "," << times[0] << '\n';
    std::cout << "FibHeap," << method << "," << N << ",1," << runs << "," << times[1] << '\n';
    std::cout << std::flush;
}

template<template<typename> typename T>
void runMulti(size_t N, const char* method) {
    long times[2] = { 0L, 0L };

    std::mt19937 gen;
    std::uniform_int_distribution<> dist;

    int seed = dist(gen);
    times[0] += benchMulti<BinaryHeap<Dummy>, T>(N, seed, 20, 5);
    times[1] += benchMulti<FibonacciHeap<Dummy>, T>(N, seed, 20, 5);

    std::cout << "BinHeap," << method << "," << N << ",100,5," << times[0] << '\n';
    std::cout << "FibHeap," << method << "," << N << ",100,5," << times[1] << '\n';
    std::cout << std::flush;
}

int main() {
    bool enabled[6] = { 1, 1, 1, 1, 1, 1 };
    std::vector<size_t> Ns;
    std::vector<size_t> Ms;

    for (size_t i = 100000; i <= MAX_SIZE; i += 100000) {
        Ns.push_back(i);
    }

    for (size_t i = 1; i <= 20; i++) {
        Ms.push_back(i * 5000);
    }

    std::cout << "struct,method,N,m,runs,total_time_ns\n";

    // benchInsert
    if (enabled[0])
        for (size_t N : Ns) {
            run<BenchInsert>(N, 5, "Insert");
        }

    // benchExtractMax
    if (enabled[1])
        for (size_t N : Ns) {
            run<BenchExtract>(N, 5, "Extract");
        }

    // benchFindMax
    if (enabled[2])
        for (size_t M : Ms) {
            runMulti<BenchFindMax>(M, "FindMax");
        }

    // benchIncreaseKey
    if (enabled[3])
        for (size_t N : Ns) {
            run<BenchIncreaseKey>(N, 1, "Increase");
        }
    // benchDecreaseKey
    if (enabled[4])
        for (size_t N : Ns) {
            run<BenchDecreaseKey>(N, 1, "Decrease");
        }
    // benchReturnSize
    if (enabled[5])
        for (size_t M : Ms) {
            runMulti<BenchReturnSize>(M, "RetSize");
        }
}
