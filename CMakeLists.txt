cmake_minimum_required(VERSION 3.6)
project(CacheObliviousRangeTree)

set(CMAKE_CXX_STANDARD 11)

set(HDR
        RangeTree.h
        TreePrinter.h
        )
set(SRC
        RangeTree.cpp
        TreePrinter.cpp
        main.cpp
        )

set(SOURCE_FILES ${SRC} ${HDR})

add_executable(CacheObliviousRangeTree ${SOURCE_FILES})