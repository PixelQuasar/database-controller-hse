cmake_minimum_required(VERSION 3.26)

add_library(Parser STATIC Parser.cpp)
target_include_directories(Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ParserTests Parser_ut.cpp)
target_link_libraries(ParserTests PRIVATE Parser gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ParserTests)