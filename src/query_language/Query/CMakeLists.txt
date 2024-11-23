cmake_minimum_required(VERSION 3.26)

add_library(Query STATIC Query.cpp)
target_include_directories(Query PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(QueryTests Query_ut.cpp)
target_link_libraries(QueryTests PRIVATE Query gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(QueryTests)