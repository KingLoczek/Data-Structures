#include "CuckooHashTable.hpp"
#include "HashTable.hpp"
#include "Hasher.hpp"
#include "ListClosedAddressingHashTable.hpp"
#include "OpenAddressingHashTable.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <set>

template<>
struct Hasher<int> {
    uint64_t operator()(int k) {
        return static_cast<uint64_t>(k);
    }
};

// Function to perform stress testing on a hash table
void stressTest(HashTable<int, int>* table) {
    long seed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(1, (1 << 15) - 1);
    std::set<int> keys;

    // Insert random keys into the table
    while (keys.size() <= 1024) {
        int key = dist(gen);
        auto p = keys.insert(key);
        if (p.second) {
            table->insert(key, key);
        }
    }

    size_t problems = 0;
    // Remove keys from the table and check if the value matches the key
    for (auto key : keys) {
        int v = table->remove(key);
        if (v != key) {
            problems++;
        }
    }

    std::cout << "   problems = " << problems << '\n';
}

int main() {
    std::cout << "HashTable implementation:\n   0. Closed Addressing with List\n   1. Open Addressing\n   2. Cuckoo hashing\n > ";
    size_t impl;
    std::cin >> impl;

    HashTable<int, int>* table;
    switch (impl) {
        case 0:
            table = new ListClosedAddressingHashTable<int, int>(4);
            break;
        case 1:
            table = new OpenAddressingHashTable<int, int>(4);
            break;
        case 2:
            table = new CuckooHashTable<int, int>(4);
            break;
    }

    std::cout << "Operation:\n   0. q - quit\n   1. a <key> <value> - add\n   2. r <key> - remove\n   3. d - state of the table\n   4.t - test\n";
    char op = 0;
    while (op != 'q') {
        std::cout << " > ";
        std::cin >> op;
        switch (op) {
            case 'q': break;
            case 'a': {
                int k, v;
                std::cin >> k >> v;
                table->insert(k, v);
                break;
            }
            case 'r': {
                int k;
                std::cin >> k;
                int v = table->remove(k);
                std::cout << "   v = " << v << '\n';
                break;
            }
            case 'd': {
                table->debug(std::cout);
                break;
            }
            case 't': {
                stressTest(table);
                break;
            }
        }
    }
    
    std::cout << "bye.\n";
    delete table;

    return 0;
}
