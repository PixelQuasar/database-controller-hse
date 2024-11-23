cmake_minimum_required(VERSION 3.26)

add_library(Executor STATIC Executor.cpp)
target_include_directories(Executor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ExecutorTests Executor_ut.cpp)
target_link_libraries(ExecutorTests PRIVATE 
    Executor 
    Calculator 
    Parser 
    gtest 
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(ExecutorTests)