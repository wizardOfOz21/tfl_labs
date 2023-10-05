cmake_minimum_required(VERSION 3.14)
project(_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_ROOT "/Program Files (x86)/googletest-distribution")
find_package(GTest REQUIRED)

add_executable(tset_test tset_test.cpp)
add_executable(graph_test graph_test.cpp)
target_include_directories(tset_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(tset_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(tset_test PRIVATE GTest::GTest)
target_include_directories(graph_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(graph_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(graph_test PRIVATE GTest::GTest)
