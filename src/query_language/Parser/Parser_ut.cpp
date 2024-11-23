#include "Parser.h"
#include <gtest/gtest.h>

using namespace database;

class ParserTest : public ::testing::Test {};

TEST_F(ParserTest, ParseCreateTable) {
    auto stmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 2);
    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, "INT");
    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, "VARCHAR");
}

TEST_F(ParserTest, ParseInsert) {
    auto stmt = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->tableName, "Test");
    ASSERT_EQ(insertStmt->values.size(), 2);
    EXPECT_EQ(insertStmt->values[0], "1");
    EXPECT_EQ(insertStmt->values[1], "\"Alice\"");
} 