cmake_minimum_required(VERSION 3.26)

add_library(Database STATIC Database.cpp)
target_include_directories(Database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(DatabaseTests Database_ut.cpp)
target_link_libraries(DatabaseTests PRIVATE Database Table Calculator gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(DatabaseTests)