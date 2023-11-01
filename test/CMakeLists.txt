cmake_minimum_required(VERSION 3.14)
project(_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_ROOT "/Program Files (x86)/googletest-distribution")
find_package(GTest REQUIRED)