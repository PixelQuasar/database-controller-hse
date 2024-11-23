file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(ResultTests ${TEST_SOURCES})

target_link_libraries(ResultTests PRIVATE Result gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ResultTests)