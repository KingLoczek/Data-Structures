#ifndef OPEN_ADDRESSING_HASH_TABLE_HPP
#define OPEN_ADDRESSING_HASH_TABLE_HPP

#include "HashTable.hpp"
#include "Hasher.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>

template<typename K, typename V>
class OpenAddressingHashTable : public HashTable<K, V> {

        // https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/HashMap.java#L336
        uint64_t hash(K key) {
            uint64_t h = Hasher<K>()(key);
            return h ^ (h >> 16);
        }

        // from H. Warren "Hacker's Delight"
        // Calculate the next power of two greater than or equal to x
        static size_t clp2(size_t x) {
            x = x - 1;
            x = x | (x >> 1);
            x = x | (x >> 2);
            x = x | (x >> 4);
            x = x | (x >> 8);
            x = x | (x >> 16);
            x = x | (x >> 32);
            return x + 1;
        }

        // Calculate the threshold for resizing the hash table
        static size_t threshold(size_t cap) {
            return (size_t)((double)cap * 0.75);
        }

        class Node {
            public:
                K mKey;
                V mValue;
                uint64_t mHash;

                Node(K key, V value, uint64_t h) : mKey(key), mValue(value), mHash(h) {}
        };

        size_t index(uint64_t h) {
            return h & (mCap - 1);
        }

        void resize() {
            if (!mArr) {
                mArr = new Node*[mCap];
                memset(mArr, 0, mCap * sizeof(Node*));
                assert(mArr && "Out of memory");
            } else {
                if (mSize >= mThr) {
                    size_t cap = mCap;
                    mCap = mCap << 1;
                    mThr = mThr << 1;
                    Node** tmp = new Node*[mCap];
                    memset(tmp, 0, mCap * sizeof(Node*));
                    assert(tmp && "Out of memory");
                    for (size_t i = 0; i < cap; i++) {
                        doInsert(tmp, mArr[i]);
                    }
                    delete[] mArr;
                    mArr = tmp;
                }
            }
        }

        // Insert a node into the given array using linear probing
        void doInsert(Node** dst, Node* node) {
            if (!node) return;
            size_t i = index(node->mHash);
            // linear probing
            while (dst[i]) {
                i = (i + 1) & (mCap - 1); // C = 1
            }
            dst[i] = node;
        }

        Node* doFind(K key, uint64_t h) {
            if (!mArr) return nullptr;
            size_t i = index(h);
            if (!mArr[i]) return nullptr;
            while (mArr[i]->mKey != key) {
                i = (i + 1) & (mCap - 1);
                if (!mArr[i]) return nullptr;
            }
            return mArr[i];
        }

        ssize_t indexOf(K key, uint64_t h) {
            if (!mArr) return -1;
            size_t i = index(h);
            while (mArr[i] && mArr[i]->mKey != key) {
                i = (i + 1) & (mCap - 1);
            }
            return mArr[i] ? i : -1;
        }
    
        // Open Addressing linear probing C=1
        //https://github.com/carrotsearch/hppc/blob/38f14e8ebae646f957b7afea4ff1cd1d74701b61/hppc/src/main/templates/com/carrotsearch/hppc/KTypeVTypeHashMap.java#L129
        size_t mCap;      // Capacity of the hash table
        size_t mThr;      // Threshold for resizing the hash table
        size_t mSize;     // Current number of elements in the hash table
        Node** mArr;      // Array of pointers to nodes
        
    public:
        OpenAddressingHashTable() : mCap(16), mThr(threshold(mCap)), mSize(0), mArr(nullptr) {}

        OpenAddressingHashTable(size_t cap) : mCap(clp2(cap)), mThr(threshold(mCap)), mSize(0), mArr(nullptr) {}

        virtual ~OpenAddressingHashTable() {
            delete[] mArr;
            mSize = 0;
        }

        virtual void insert(K key, V value) override {
            uint64_t h = hash(key);
            assert(!doFind(key, h) && "Duplicate key");
            resize();
            Node* node = new Node(key, value, h);
            assert(node && "Out of memory");
            doInsert(mArr, node);
            mSize++;
        }

        virtual V remove(K key) override {
            uint64_t h = hash(key);
            ssize_t i = indexOf(key, h);
            assert(i >= 0 && "Key not present");
            Node* evicted = mArr[i];
            V value = evicted->mValue;
            delete evicted;
            mArr[i] = nullptr;
            mSize--;
            return value;
        }

        virtual void debug(std::ostream& os) override {
            os << "OpenAddressHashTable\n   size = " << mSize << "\n   cap = " << mCap << "\n   thr = " << mThr << "\n   arr = \n";
            for (size_t i = 0; i < mCap; i++) {
                Node* node = mArr[i];
                os << "      [" << i << "] ";
                if (node) {
                    os << "(" << node->mKey << ", " << node->mValue << ")#" << node->mHash << "[" << index(node->mHash) << "]\n";
                } else {
                    os << "null\n";
                }
            }
        }
};

#endif
