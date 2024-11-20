cmake_minimum_required(VERSION 3.26)
project(database_controller_hse)

set(CMAKE_CXX_STANDARD 20)

add_library(Calculator STATIC src/calculator/Calculator.cpp)
target_include_directories(Calculator PUBLIC src/calculator)


set(SOURCE_FILES
        main.cpp
        src/database/Database/Database.cpp
        src/database/Table/Table.cpp
        src/query_language/Executor/Executor.cpp
        src/query_language/Query/Query.cpp
        src/database/Result/Result.cpp
)

add_executable(database ${SOURCE_FILES})

target_link_libraries(database PRIVATE Calculator)

add_subdirectory(src/calculator/ut)
