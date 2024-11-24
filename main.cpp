#include "src/query_language/Parser/Parser.h"
#include "src/query_language/Executor/Executor.h"
#include "src/database/Database/Database.h"
#include "src/query_language/Query/Query.h"
#include "src/query_language/Result/Result.h"
#include "src/query_language/AST/SQLStatement.h"
#include "src/query_language/Executor/Executor.h"
#include <iostream>

int main() {
//    std::string raw_str = "CREATE TABLE Persons (\n"
//                        "    PersonID int,\n"
//                        "    LastName varchar(255),\n"
//                        "    FirstName varchar(255),\n"
//                        "    Address varchar(255),\n"
//                        "    City varchar(255)\n"
//                        ");";

    std::string raw_str = "SELECT name, password FROM Persons WHERE age > 6;";

    auto db = database::Database();

    auto parsed = database::Parser::parse(raw_str);

    auto exectutor = database::Executor(db);


    return 0;
}