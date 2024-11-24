#include "src/query_language/Parser/Parser.h"
#include "src/query_language/Executor/Executor.h"
#include "src/database/Database/Database.h"
#include "src/query_language/Query/Query.h"
#include <iostream>

int main() {
//    std::string raw_str = "CREATE TABLE Persons (\n"
//                        "    PersonID int,\n"
//                        "    LastName varchar(255),\n"
//                        "    FirstName varchar(255),\n"
//                        "    Address varchar(255),\n"
//                        "    City varchar(255)\n"
//                        ");";

    std::string raw_str = "SELECT * FROM Persons;";



    database::Query query(raw_str);

    return 0;
}