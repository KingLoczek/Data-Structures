add_executable(BenchHeaps HeapsBench/bench.cpp)
target_include_directories(BenchHeaps PRIVATE "${CMAKE_SOURCE_DIR}/Heaps")
