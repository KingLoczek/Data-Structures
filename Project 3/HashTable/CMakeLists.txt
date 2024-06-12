add_executable(htables
    driver.cpp
)
target_include_directories(htables PRIVATE "${CMAKE_SOURCE_DIR}/HashTable")
