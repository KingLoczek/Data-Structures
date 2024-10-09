#include "CuckooHashTable.hpp"
#include "HashTable.hpp"
#include "ListClosedAddressingHashTable.hpp"
#include "OpenAddressingHashTable.hpp"
#include <cstddef>
#include <vector>
#include <set>
#include <random>
#include <chrono>
#include <iostream>
#include <iterator>

template<>
struct Hasher<int> {
    uint64_t operator()(int k) {
        return static_cast<uint64_t>(k);
    }
};

template<template<typename, typename> typename T>
HashTable<int, int>** makeTables(size_t N, size_t m) {
    HashTable<int, int>** tables = new HashTable<int, int>*[m];
    for (size_t i = 0; i < m; i++)
        tables[i] = new T<int, int>(N);
    return tables;
}

long benchInsert(long seed, size_t N, size_t m, HashTable<int, int>** tables) {
    std::uniform_int_distribution<> dist(1, (1 << 16) - 1);
    std::vector<int> theKeys;
    size_t stride = m / 16;
    for (size_t i = 0; i < m; i += stride) {
        std::set<int> keys;
        std::mt19937 gen(seed + (i ^ i));

        while (keys.size() <= N) {
            int key = dist(gen);
            if (keys.insert(key).second) {
                for (size_t j = 0; j < stride; j++)
                    tables[i + j]->insert(key, (key >> 3) ^ key);
            }
        }

        int theKey = dist(gen);
        while (!keys.insert(theKey).second) {
            theKey = dist(gen);
        }

        for (size_t j = 0; j < stride; j++)
            theKeys.push_back(theKey);
    }

    auto begin = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < m; i++) {
        int theKey = theKeys[i];
        tables[i]->insert(theKey, (theKey >> 3) ^ theKey);
    }
    auto end = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < m; i++) {
        delete tables[i];
    }
    delete[] tables;
    return std::chrono::duration_cast<std::chrono::nanoseconds>((end - begin) / m).count();
}

long benchRemove(long seed, size_t N, size_t m, HashTable<int, int>** tables) {
    std::uniform_int_distribution<> dist(1, (1 << 16) - 1);
    std::vector<int> theKeys;
    for (size_t i = 0; i < m; i += (m / 16)) {
        std::set<int> keys;
        std::mt19937 gen(seed + (i * 123));

        while (keys.size() <= N) {
            int key = dist(gen);
            if (keys.insert(key).second) {
                for (size_t j = 0; j < (m / 16); j++)
                    tables[i + j]->insert(key, (key >> 3) ^ key);
            }
        }

        std::uniform_int_distribution<size_t> dist2(0, keys.size() - 1);
        size_t n = dist2(gen);
        auto it = keys.begin();
        std::advance(it, n);
        int theKey = *it;

        for (size_t j = 0; j < (m / 16); j++)
            theKeys.push_back(theKey);
    }


    auto begin = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < m; i++) {
        int theKey = theKeys[i];
        tables[i]->remove(theKey);
    }
    auto end = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < m; i++) {
        delete tables[i];
    }
    delete[] tables;
    return std::chrono::duration_cast<std::chrono::nanoseconds>((end - begin) / m).count();
}

int main() {
    std::vector<size_t> Ns{500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
    size_t m = 1024;
    std::cout << "method,table,N,m,unit_time_ns\n";
    for (size_t N : Ns) {
        long seed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        long time;
        time = benchInsert(seed, N, m, makeTables<ListClosedAddressingHashTable>(N, m));
        std::cout << "insert,close," << N << "," << m << "," << time << std::endl;
        time = benchInsert(seed, N, m, makeTables<OpenAddressingHashTable>(N, m));
        std::cout << "insert,open," << N << "," << m << "," << time << std::endl;
        time = benchInsert(seed, N, m, makeTables<CuckooHashTable>(N, m));
        std::cout << "insert,cuckoo," << N << "," << m << "," << time << std::endl;
        time = benchRemove(seed, N, m, makeTables<ListClosedAddressingHashTable>(N, m));
        std::cout << "remove,close," << N << "," << m << "," << time << std::endl;
        time = benchRemove(seed, N, m, makeTables<OpenAddressingHashTable>(N, m));
        std::cout << "remove,open," << N << "," << m << "," << time << std::endl;
        time = benchRemove(seed, N, m, makeTables<CuckooHashTable>(N, m));
        std::cout << "remove,cuckoo," << N << "," << m << "," << time << std::endl;
    }
}
