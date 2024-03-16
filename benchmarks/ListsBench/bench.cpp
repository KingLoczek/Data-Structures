#include <benchmark/benchmark.h>
#include <iostream>
#include "arraylist.h"
#include "SinglyLinkedHead.h"
#include "SinglyLinkedList.h"
#include "DoublyLinkedList.h"
#include <random>

using ItemT = int;

template<class T>
void bench_list_append(benchmark::State& state) {
    for (auto _ : state) {
        T list;

        for (int i = 0; i < state.range(0); i++) {
            list.add(i);
            benchmark::ClobberMemory();
        }

        benchmark::DoNotOptimize(list);
    }
    state.SetItemsProcessed(state.range(0)*state.iterations());
    state.SetComplexityN(state.range(0));
}

template<class T>
void bench_list_prepend(benchmark::State& state) {
    for (auto _ : state) {
        T list;

        for (int i = 0; i < state.range(0); i++) {
            list.push(i);
            benchmark::ClobberMemory();
        }

        benchmark::DoNotOptimize(list);
    }

    state.SetItemsProcessed(state.range(0)*state.iterations());
    state.SetComplexityN(state.range(0));
}

template<class T>
void bench_list_add(benchmark::State& state) {
    state.PauseTiming();

    int size = state.range(0) - 2;
    int* indicies = new int[size];

    indicies[0] = 1;

    std::mt19937 gen;
    for (int i = 1; i < size; i++) {
        std::uniform_int_distribution<> dist(1, i);
        indicies[i] = dist(gen);
    }

    state.ResumeTiming();

    for (auto _ : state) {
        T list;

        list.add(0xDEADBEEF);
        list.add(0xCAFEBABE);
        for (int i = 0; i < state.range(0) - 2; i++) {
            list.add(indicies[i], i);
            //std::cout << list.size() << '\n';
            benchmark::ClobberMemory();
        }

        benchmark::DoNotOptimize(list);
    }
}

static const auto ITERS = std::vector<long int>{ 5000, 8000, 10000, 16000, 20000, 40000, 60000, 100000};
// This can take a really long time
//static const auto ITERS = std::vector<long int>{ 5000, 8000, 10000, 16000, 20000, 40000, 60000, 100000, 250000, 500000, 750000, 1000000};
static void ApplyIterations(benchmark::internal::Benchmark* b) {
    for (auto size : ITERS) {
        b->Arg(size);
    }
}

BENCHMARK(bench_list_append<ArrayList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_append<SinglyLinkedHead<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_append<SinglyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_append<DoublyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();

BENCHMARK(bench_list_prepend<ArrayList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_prepend<SinglyLinkedHead<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_prepend<SinglyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_prepend<DoublyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();

BENCHMARK(bench_list_add<ArrayList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_add<SinglyLinkedHead<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_add<SinglyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();
BENCHMARK(bench_list_add<DoublyLinkedList<ItemT>>)->Apply(ApplyIterations)->Complexity();

BENCHMARK_MAIN();
