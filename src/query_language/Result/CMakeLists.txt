cmake_minimum_required(VERSION 3.26)

add_library(Result STATIC Result.cpp)
target_include_directories(Result PUBLIC .)

add_executable(ResultTests Result_ut.cpp)
target_link_libraries(ResultTests PRIVATE Result gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ResultTests)