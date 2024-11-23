file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(QueryTests ${TEST_SOURCES})

target_link_libraries(QueryTests PRIVATE Query gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(QueryTests)