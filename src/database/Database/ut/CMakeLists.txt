file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(DatabaseTests ${TEST_SOURCES})

target_link_libraries(DatabaseTests PRIVATE Database gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(DatabaseTests)