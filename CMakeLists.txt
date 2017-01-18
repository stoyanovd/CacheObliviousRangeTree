cmake_minimum_required(VERSION 3.6)
project(CacheObliviousRangeTree)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp RangeTree.cpp RangeTree.h)
add_executable(CacheObliviousRangeTree ${SOURCE_FILES})