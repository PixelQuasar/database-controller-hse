file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(ExecutorTests ${TEST_SOURCES})

target_link_libraries(ExecutorTests PRIVATE Executor gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ExecutorTests)