cmake_minimum_required(VERSION 3.17)

project(mcnbt_example)

link_libraries(mcnbt::mcnbt)

add_executable(base_example base_example.cpp)
add_executable(boundary_texst boundary_test.cpp)
if(MCNBT_ENABLE_GZIP)
    add_executable(de_compress_example de_compress_example.cpp)
endif()
add_executable(fast_way_example fast_way_example.cpp)
add_executable(read_write_example read_write_example.cpp)
add_executable(single_block_mcstructure_example single_block_mcstructure_example.cpp)
add_executable(snbt_example snbt_example.cpp)
