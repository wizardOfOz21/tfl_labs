cmake_minimum_required(VERSION 3.14)
project(_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_ROOT "/Program Files (x86)/googletest-distribution")
find_package(GTest REQUIRED)

add_executable(tset_test tset_test.cpp)
add_executable(graph_test graph_test.cpp)
add_executable(to_regex_test AutomataToRegex_test.cpp)
add_executable(basic_test basic_test.cpp)

target_include_directories(tset_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(tset_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(tset_test PRIVATE GTest::GTest)

target_include_directories(graph_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(graph_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(graph_test PRIVATE GTest::GTest)

target_include_directories(to_regex_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(to_regex_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(to_regex_test PRIVATE GTest::GTest)

target_include_directories(basic_test PUBLIC ${SIMPLIFIER_INCLUDE_DIRS})
target_link_libraries(basic_test PRIVATE ${SIMPLIFIER_LIBRARIES})
target_link_libraries(basic_test PRIVATE GTest::GTest)
