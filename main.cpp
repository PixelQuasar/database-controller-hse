#include <iostream>

#include "src/database/Database/Database.h"
#include "src/query_language/AST/SQLStatement.h"
#include "src/query_language/Executor/Executor.h"
#include "src/query_language/Parser/Parser.h"
#include "src/query_language/Query/Query.h"
#include "src/query_language/Result/Result.h"
#include "src/types.h"

using namespace database;

int main() {
    auto db = database::Database();
    auto executor = database::Executor(db);

    std::string create_str =
        "CREATE TABLE Employees ("
        "    ID INT,"
        "    FirstName VARCHAR,"
        "    LastName VARCHAR,"
        "    Age INT,"
        "    Salary DOUBLE,"
        "    IsManager BOOL,"
        "    IsFullTime BOOL,"
        "    YearsOfService DOUBLE,"
        "    PerformanceScore INT"
        ");";

    std::vector<std::string> insert_strs = {
            "INSERT INTO Employees VALUES (4, \"Alice\", \"Williams\", 20 + (2 * "
            "5), 55000.0, true && (false || true), (true && true) || false, 3.5, "
            "(90 + 5) - (10 / 2));"
    };

    executor.execute(database::Parser::parse(create_str));

    for (auto insert_str : insert_strs) {
        std::cout << executor.execute(database::Parser::parse(insert_str)).get_error_message() << std::endl;
    }


    std::string select_str = "SELECT * FROM Employees";

     auto result = executor.execute(database::Parser::parse(select_str));

     if (!result.is_ok()) {
         throw std::runtime_error("select error " + result.get_error_message());
     }

     for (auto row : result.get_payload()) {
         std::cout << database::dBTypeToString(row["FirstName"]) << " "
                   << database::dBTypeToString(row["Salary"]) << std::endl;
     }

    return 0;
}
