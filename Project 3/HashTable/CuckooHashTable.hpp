#ifndef CUCKOO_HASH_TABLE_HPP
#define CUCKOO_HASH_TABLE_HPP

#include "HashTable.hpp"
#include "Hasher.hpp"
#include <cstdint>
#include <cassert>
#include <cstring>

template<typename K, typename V>
class CuckooHashTable : public HashTable<K, V> {

        // TODO: replace hash function with something that has another parameter
        // https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/
        // ^ - use this + some simple xorshift rng to generate the parameters and it should be good to go
        //
        // xorshift https://doi.org/10.18637%2Fjss.v008.i14


        //static uint64_t hash1(K key) {
        //    uint64_t h = Hasher<K>()();
        //    return h ^ (h >> 16);
        //}
        
        //static uint64_t hash2(K key) {
        //    uint64_t h = Hasher<K>()();
        //    h *= 0x9e3779b97f4a7c15; // golden ration
        //    return h ^ (h >> 16);
        //}

        // from H. Warren "Hacker's Delight"
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

        static size_t threshold(size_t cap) {
            return (size_t)((double)cap * 0.5);
        }

        class Node {
            public:
                K mKey;
                V mValue;
                uint64_t mHash1;
                uint64_t mHash2;

                Node(K key, V value, uint64_t h1, uint64_t h2) : mKey(key), mValue(value), mHash1(h1), mHash2(h2) {
                }
        };

        void resize() {
            if (!mArr) {
                mArr = new Node*[mCap * 2];
                assert(mArr && "Out of memory!");
                memset(mArr, 0, mCap * 2 * sizeof(Node*));
            } else {
                if (mSize >= mThr) {
                    size_t cap = mCap;
                    mCap = mCap << 1;
                    mThr = mThr << 1;
                    rehash(mArr, cap);
                }
            }
        }

        Node* tryInsert(Node** dst, Node* node) {
            //printf("trying to insert (%d, %d)\n", node->mKey, node->mValue);
            Node* iter = node;
            for (size_t j = 0; j < mCap; j++) {
                Node* fresh = iter;
                size_t i1 = index1(iter->mHash1);
                if (dst[i1]) {
                    Node* evicted = dst[i1];
                    size_t i2 = index2(evicted->mHash2);
                    iter = dst[i2];
                    dst[i2] = evicted;
                    dst[i1] = fresh;
                    if (!iter) return nullptr;
                } else {
                    dst[i1] = fresh;
                    return nullptr;
                }
            }
            return iter;
        }

        Node** rehash(Node** src, size_t nSrc) {
            Node** tmp = new Node*[mCap * 2];
            assert(tmp && "Out of memory!");
            memset(tmp, 0, mCap * 2 * sizeof(Node*));

            // advance mX, mY
            xorshift1();
            xorshift2();

            // TODO: bound on how many rehashes?
            for (size_t i = 0; i < nSrc * 2; i++) {
                Node* node = src[i];
                if (node) {
                    //printf("rehashing (%d, %d)\n", node->mKey, node->mValue);
                    node->mHash1 = hash1(node->mKey);
                    node->mHash2 = hash2(node->mKey);

                    if (tryInsert(tmp, node)) {
                        // failed attempt
                        i = -1;
                        // advance mX, mY again
                        xorshift1();
                        xorshift2();
                        memset(tmp, 0, mCap * 2 * sizeof(Node*));
                    }
                }
            }
            delete[] mArr;
            mArr = tmp;
            return mArr;
        }

        void doInsert(Node** dst, Node* node) {
            if (!node) return;
            for (size_t i = 0; i < 8; i++) {
                //printf("try to insert (%d, %d)\n", node->mKey, node->mValue);
                if (!(node = tryInsert(dst, node))) return;
                dst = rehash(dst, mCap);
            }
            assert(false && "max insert tries reached");
        }

        size_t index1(uint64_t h) {
            return h & (mCap - 1);
        }

        size_t index2(uint64_t h) {
            return (h & (mCap - 1)) + mCap;
        }
        
        ssize_t indexOf(K key, uint64_t h1, uint64_t h2) {
            if (!mArr) return -1;
            Node* node = mArr[index1(h1)];
            if (node && node->mKey == key) return index1(h1);
            node = mArr[index2(h2)];
            if (node && node->mKey == key) return index2(h2);
            return -1;
        }

        uint64_t xorshift1() {
            // yˆ=y<<a; yˆ=y>>b; yˆ=y<<c;
            // 13, 17, 5
            mX ^= mX << 13;
            mX ^= mX >> 17;
            return (mX ^= mX << 5);
        }

        uint64_t xorshift2() {
            // yˆ=y>>c; yˆ=y>>a; yˆ=y<<b;
            //  9, 1,27
            mY ^= mY >> 27;
            mY ^= mY >> 9;
            return (mY ^= mY << 1);
        }

        uint64_t hash1(K key) {
            // first round of hashing
            uint64_t h = Hasher<K>()(key);
            uint32_t low = (uint32_t)h;
            uint32_t high = (uint32_t)(h >> 32);
            h = (uint32_t)((mX * low + 0x4175746841594D44 * high + 0xDEADBEEFBEEFDEAD) >> 32);
            // apply xorshift round to mX
            uint64_t x = mX << 1;
            x ^= x >> 7;
            x ^= x << 9;
            // second round of hashing
            h |= (x * low + 0x47656E6E496E746C * high + 0xDEADCAFECAFEDEAD) & 0xFFFFFFFF;
            // spread bits
            return h ^ (h >> 16);
        }

        uint64_t hash2(K key) {
            // first round of hashing
            uint64_t h = Hasher<K>()(key);
            uint32_t low = (uint32_t)h;
            uint32_t high = (uint32_t)(h >> 32);
            h = ((mY * low + 0x47656E496F74656C * high + 0xDEADBABEBABEDEAD) >> 32);
            // apply xorshift round to mY
            uint64_t a = mY << 4;
            a ^= a >> 7;
            a ^= a << 19;
            // second round of hashing
            h |= (a * low + 0x47656F64654E5343 * high + 0xDEADC0DEC0DEDEAD) & 0xFFFFFFFF;
            // spread bits
            return h ^ (h >> 16);
        }

        size_t mCap;
        size_t mThr;
        size_t mSize;
        uint64_t mX = 88675123;
        uint64_t mY = 362436069;

        Node** mArr;

    public:
        CuckooHashTable() : mCap(16), mThr(threshold(mCap)), mSize(0), mArr(nullptr) {
        }

        CuckooHashTable(size_t cap) : mCap(clp2(cap)), mThr(threshold(mCap)), mSize(0), mArr(nullptr) {
        }

        virtual ~CuckooHashTable() override {
            delete[] mArr;
            mArr = nullptr;
            mSize = 0;
        }

        virtual void insert(K key, V value) override {
            resize();
            uint64_t h1 = hash1(key);
            uint64_t h2 = hash2(key);
            assert(indexOf(key, h1, h2) < 0 && "Duplicate key");
            h1 = hash1(key);
            h2 = hash2(key);
            Node* node = new Node(key, value, h1, h2);
            assert(node && "Out of memory");
            doInsert(mArr, node);
            mSize++;
        }

        virtual V remove(K key) override {
            uint64_t h1 = hash1(key);
            uint64_t h2 = hash2(key);
            ssize_t i = indexOf(key, h1, h2);
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
                os << "      [1][" << i << "] ";
                if (node) {
                    os << "(" << node->mKey << ", " << node->mValue << ")#" << node->mHash1 << "[" << index1(node->mHash1) << "]\n";
                } else {
                    os << "null\n";
                }
            }

            for (size_t i = 0; i < mCap; i++) {
                Node* node = mArr[i + mCap];
                os << "      [2][" << i << "] ";
                if (node) {
                    os << "(" << node->mKey << ", " << node->mValue << ")#" << node->mHash2 << "[" << index2(node->mHash2) << "]\n";
                } else {
                    os << "null\n";
                }
            }
        }
};

#endif
