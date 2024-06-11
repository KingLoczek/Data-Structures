#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <ostream>

template<typename K, typename V>
class HashTable {
    public:

        virtual ~HashTable() {}

        virtual void insert(K key, V value) = 0;
        virtual V remove(K key) = 0;

        // Output the internal state of the hash table for debugging
        virtual void debug(std::ostream& os) = 0;
};

#endif
