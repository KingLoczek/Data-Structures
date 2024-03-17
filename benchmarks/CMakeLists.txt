find_package(benchmark REQUIRED)

add_executable(ListsBench ListsBench/bench.cpp)
target_include_directories(ListsBench PRIVATE "${CMAKE_SOURCE_DIR}/List")
target_link_libraries(ListsBench benchmark::benchmark)
target_compile_options(ListsBench PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

add_executable(SListsBench ListsBench/sbench.cpp)
target_include_directories(SListsBench PRIVATE "${CMAKE_SOURCE_DIR}/List")
target_compile_options(SListsBench PRIVATE "$<$<CONFIG:RELEASE>:-O3>")
