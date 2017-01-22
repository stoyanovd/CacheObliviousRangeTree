cmake_minimum_required(VERSION 3.2)

### Hunter
include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.17.17.tar.gz"
        SHA1 "b561a1c80731bd3c6dab2e54f76178e734249688"
)

project(CacheObliviousRangeTree)

set(CMAKE_CXX_STANDARD 11)

### GLOG
hunter_add_package(glog)
find_package(glog CONFIG REQUIRED)

include_directories(${GLOG_INCLUDE_DIRS})


enable_testing()
### GTEST
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

set(HDR
        Node.h
        RangeTree.h
        TreePrinter.h
        Analytics.h
        Task.h
        InputCreator.h
        )
set(SRC
        Node.cpp
        RangeTree.cpp
        TreePrinter.cpp
        InputCreator.cpp
        )

set(SOURCE_FILES main.cpp ${SRC} ${HDR})

add_executable(CacheObliviousRangeTree ${SOURCE_FILES})
target_link_libraries(CacheObliviousRangeTree glog::glog)


add_executable(runTests runTests.cpp ${SRC} ${HDR})
target_link_libraries(runTests glog::glog GTest::main GTest::gtest)

add_test(TestAllWorks runTests ${SRC} ${HDR})
