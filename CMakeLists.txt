cmake_minimum_required(VERSION 3.26)
project(database_controller_hse)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        main.cpp
        src/database/Database/Database.cpp
        src/database/Result/Result.cpp
        src/database/Table/Table.cpp
        src/calculator/Calculator/Calculator.cpp
        src/query_language/Executor/Executor.cpp
        src/query_language/Executor/Executor.h
        src/query_language/Query/Query.cpp
        src/query_language/Query/Query.h
)

add_executable(database ${SOURCE_FILES})
