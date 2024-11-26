cmake_minimum_required(VERSION 3.26)

add_library(Logger STATIC Logger.cpp)
target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(LoggerTests Logger_ut.cpp)
target_link_libraries(LoggerTests PRIVATE Logger Logger gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(LoggerTests)