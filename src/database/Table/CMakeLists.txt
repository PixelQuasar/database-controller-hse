cmake_minimum_required(VERSION 3.26)

add_library(Table STATIC Table.cpp)
target_include_directories(Table PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TableTests Table_ut.cpp)
target_link_libraries(TableTests PRIVATE Table gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(TableTests)