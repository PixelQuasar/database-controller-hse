cmake_minimum_required(VERSION 3.26)

add_library(Calculator STATIC Calculator.cpp)
target_include_directories(Calculator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(CalculatorTests Calculator_ut.cpp)
target_link_libraries(CalculatorTests PRIVATE Calculator gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(CalculatorTests)
