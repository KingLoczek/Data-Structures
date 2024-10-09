#ifndef LIST_CLOSED_ADDRESSING_HASH_TABLE_HPP
#define LIST_CLOSED_ADDRESSING_HASH_TABLE_HPP 

#include "HashTable.hpp"
#include "Hasher.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>

template<typename K, typename V>
class ListClosedAddressingHashTable : public HashTable<K, V> {
        class TableEntry {
            public:
                K mKey;
                V mValue;
                TableEntry* mNext; // Pointer to the next entry in the chain (for collision resolution)
                uint64_t mHash;

                TableEntry(K key, K value, uint64_t hash) : mKey(key), mValue(value), mHash(hash), mNext(nullptr) {}
        };

        TableEntry** mArr; // Array of pointers to TableEntry objects (buckets)
        size_t mCap; // Capacity of the hash table (number of buckets)
        size_t mThr; // Threshold for resizing the table
        size_t mSize; // Number of elements currently in the table

        // load factor = n/m = mSize/mCap
        void doInsert(TableEntry** dst, TableEntry* entry) {
            if (entry) {
                size_t j = index(entry->mHash);
                entry->mNext = dst[j];
                dst[j] = entry;
            }
        }

        void resize() {
            if (!mArr) {
                mArr = new TableEntry*[mCap];
                assert(mArr && "Out of memory!");
                memset(mArr, 0, mCap * sizeof(TableEntry*));
            } else {
                if (mSize == mThr) {
                    mCap = mCap << 1;
                    mThr = mThr << 1;
                    TableEntry** tmp = new TableEntry*[mCap];
                    assert(tmp && "Out of memory!");
                    memset(tmp, 0, mCap * sizeof(TableEntry*));
                    for (size_t i = 0; i < mSize; i++) {
                        TableEntry* iter = mArr[i];
                        while (iter) {
                            TableEntry* next = iter->mNext;
                            doInsert(tmp, iter);
                            iter = next;
                        }
                    }
                    delete[] mArr;
                    mArr = tmp;
                }
            }
        }

        bool noDupedKey(uint64_t h, K key) {
            TableEntry* iter = mArr[index(h)];
            while (iter) {
                if (iter->mKey == key) return false;
                iter = iter->mNext;
            }
            return true;
        }

        // https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/HashMap.java#L336
        uint64_t hash(K key) {
            uint64_t h = Hasher<K>()(key);
            return h ^ (h >> 16);
        }

        size_t index(uint64_t h) {
            // h mod mCap, given that mCap is a power of 2
            return h & (mCap - 1);
        }

        TableEntry* doFind(K key, uint64_t h, TableEntry** parent) {
            if (!mArr) return nullptr;
            TableEntry* prev = nullptr;
            TableEntry* iter = mArr[index(h)];
            while (iter) {
                if (iter->mKey == key) {
                    *parent = prev;
                    return iter;
                }

                prev = iter;
                iter = iter->mNext;
            }

            *parent = nullptr;
            return nullptr;
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

    public:
        ListClosedAddressingHashTable() : mArr(nullptr), mCap(16), mThr(/*load factor = 1*/mCap), mSize(0) {}

        ListClosedAddressingHashTable(size_t cap) : mArr(nullptr), mCap(clp2(cap)), mThr(/*load factor = 1*/mCap), mSize(0) {}

        virtual ~ListClosedAddressingHashTable() {
            if (mArr) {
                for (size_t i = 0; i < mCap; i++) {
                    TableEntry* iter = mArr[i];
                    while (iter) {
                        TableEntry* next = iter->mNext;
                        delete iter;
                        iter = next;
                    }
                }
            }
            delete[] mArr;
            mArr = nullptr;
            mSize = 0;
        }

        virtual void insert(K key, V value) override {
            resize();
            uint64_t h = hash(key);
            assert(noDupedKey(key, h) && "duplicate key found");
            TableEntry* te = new TableEntry(key, value, h);
            doInsert(mArr, te);
            mSize++;
        }

        virtual V remove(K key) override {
            uint64_t h = hash(key);
            TableEntry* pte;
            TableEntry* te = doFind(key, h, &pte);
            assert(te && "key has no mapping");
            if (pte){ 
                pte->mNext = te->mNext; 
            }else { 
                mArr[index(h)] = te->mNext; 
            }
            V value = te->mValue;
            delete te;
            mSize--;
            return value;
        }

        virtual void debug(std::ostream& os) override {
            os << "ListClosedAddressingHashTable\n   size = " << mSize << "\n   cap = " << mCap << "\n   thr = " << mThr << "\n   arr = \n";
            for (size_t i = 0; i < mCap; i++) {
                TableEntry* iter = mArr[i];
                os << "      [" << i << "] ";
                if (iter) {
                    while (iter) {
                        os << "(" << iter->mKey << ", " << iter->mValue << ")#" << iter->mHash;
                        if (iter->mNext) os << " -> ";
                        iter = iter->mNext;
                    }
                    os << '\n';
                } else {
                    os << "null\n";
                }
            }
        }
};

#endif
