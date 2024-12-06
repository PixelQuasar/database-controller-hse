#include <gtest/gtest.h>

#include "../../database/Database/Database.h"
#include "../Executor/Executor.h"
#include "../Parser/Parser.h"
#include "Result.h"
using namespace database;

class ResultTest : public ::testing::Test {
   protected:
    Database db;
    Executor executor{db};
};

TEST_F(ResultTest, Result1) {
    auto createStmt = ("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 = ("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 = ("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);

    auto selectStmt = ("SELECT * FROM Test;");
    auto result = executor.execute(selectStmt);

    EXPECT_TRUE(result.is_ok());
    for (auto& row : result) {
        for (auto& [key, value] : row) {
            std::cout << key << ":" << dBTypeToString(value) << "   ";
        }
        std::cout << std::endl;
    }
}
