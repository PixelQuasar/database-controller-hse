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
    auto calc = calculator::Calculator();

    try {
        auto res = std::get<database::bytebuffer>(calc.evaluate("0x1234 + 0x1010"));
        for (auto ch : res) {
            std::cout << +ch << std::endl;
        }
    } catch (std::invalid_argument& e) {
        std::cout << "\n\n\n";
        std::cout << e.what() << std::endl;
    }

    return 0;

    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto updateStmt =
        Parser::parse("UPDATE Test SET (Age = Age + 1) WHERE ID == 1;");
    auto result = executor.execute(updateStmt);
    std::cout << result.get_error_message();

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
        "INSERT INTO Employees VALUES (1, \"John\", \"Doe\", 30, 50000.50, "
        "true, true, 5.5, 95);",
        "INSERT INTO Employees VALUES (2, \"Jane\", \"Smith\", 28, 66000.00, "
        "false, true, 8.0, 90);",
        "INSERT INTO Employees VALUES (3, \"Michael\", \"Johnson\", 45, "
        "75000.75, true, false, 10.0, 85);",
        "INSERT INTO Employees VALUES (4, \"Emily\", \"Williams\", 22, "
        "48000.25, false, true, 1.5, 88);",
        "INSERT INTO Employees VALUES (5, \"Chris\", \"Brown\", 39, 54000.00, "
        "true, true, 12.0, 92);",
        "INSERT INTO Employees VALUES (6, \"Emma\", \"Davis\", 31, 62000.50, "
        "false, false, 6.3, 80);",
        "INSERT INTO Employees VALUES (7, \"David\", \"Miller\", 36, 58000.00, "
        "true, true, 7.5, 89);",
        "INSERT INTO Employees VALUES (8, \"Sophia\", \"Taylor\", 27, "
        "49000.00, false, true, 2.0, 93);",
        "INSERT INTO Employees VALUES (9, \"Alexander\", \"Anderson\", 50, "
        "80000.00, true, false, 15.0, 87);",
        "INSERT INTO Employees VALUES (10, \"Olivia\", \"Thomas\", 40, "
        "73000.00, true, true, 9.0, 94);",
        "INSERT INTO Employees VALUES (11, \"Daniel\", \"Jackson\", 29, "
        "52000.75, false, true, 4.2, 86);",
        "INSERT INTO Employees VALUES (12, \"Sophia\", \"Martinez\", 34, "
        "61000.50, true, false, 5.0, 89);",
        "INSERT INTO Employees VALUES (13, \"Lucas\", \"Harris\", 43, "
        "69000.00, true, true, 11.0, 83);",
        "INSERT INTO Employees VALUES (14, \"Ava\", \"Clark\", 26, 53000.00, "
        "false, false, 3.5, 90);",
        "INSERT INTO Employees VALUES (15, \"Ethan\", \"Lewis\", 32, 56000.00, "
        "true, true, 8.0, 91);"};

    executor.execute(database::Parser::parse(create_str));

    for (auto insert_str : insert_strs) {
        executor.execute(database::Parser::parse(insert_str));
    }

    std::string update_str =
        "UPDATE Employees SET (Salary = 0.0) WHERE Salary > 60000.00;";

    auto update_result = executor.execute(database::Parser::parse(update_str));

    if (!update_result.is_ok()) {
        throw std::runtime_error("update error: " +
                                 update_result.get_error_message());
    }

    std::string select_str =
        "SELECT FirstName, Salary FROM Employees WHERE Salary == 0.0";

    // auto result = executor.execute(database::Parser::parse(select_str));

    // if (!result.is_ok()) {
    //     throw std::runtime_error("select error");
    // }

    // for (auto row : result.get_payload()) {
    //     std::cout << database::dBTypeToString(row["FirstName"]) << " "
    //               << database::dBTypeToString(row["Salary"]) << std::endl;
    // }

    return 0;
}
