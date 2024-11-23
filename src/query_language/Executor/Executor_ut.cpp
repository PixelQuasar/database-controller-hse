#include "Executor.h"
#include "../Parser/Parser.h"
#include "../../database/Database/Database.h"
#include <gtest/gtest.h>

using namespace database;

class ExecutorTest : public ::testing::Test {
protected:
    Database db;
    Executor executor{db};
};

TEST_F(ExecutorTest, ExecuteCreateTable) {
    auto stmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(*stmt);
    const auto& table = db.getTable("Test");
    EXPECT_EQ(table.getName(), "Test");
    ASSERT_EQ(table.getColumns().size(), 2);
    EXPECT_EQ(table.getColumns()[0].name, "ID");
    EXPECT_EQ(table.getColumns()[1].name, "Name");
}

TEST_F(ExecutorTest, ExecuteInsert) {
    auto createStmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(*createStmt);

    auto insertStmt = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    executor.execute(*insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.getData();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
} 