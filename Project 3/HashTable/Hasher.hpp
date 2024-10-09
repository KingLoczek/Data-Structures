#ifndef HASHER_HPP
#define HASHER_HPP

#include <cstdint>

template<typename T>
struct Hasher {
    // Function call operator to hash the given input
    uint64_t operator()(T x);
};

#endif
