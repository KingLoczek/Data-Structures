add_executable(BenchTables TablesBench/bench.cpp)
target_include_directories(BenchTables PRIVATE "${CMAKE_SOURCE_DIR}/HashTable")

