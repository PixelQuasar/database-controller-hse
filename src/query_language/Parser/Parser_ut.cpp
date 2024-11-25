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
    EXPECT_FALSE(createStmt->columns[0].isUnique);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, "VARCHAR");
    EXPECT_FALSE(createStmt->columns[1].isUnique);
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

TEST_F(ParserTest, ParseCreateTableWithAttributes) {
    auto stmt = Parser::parse("CREATE TABLE Test (ID INT AUTOINCREMENT, Name VARCHAR UNIQUE);");
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 2);

    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, "INT");
    EXPECT_TRUE(createStmt->columns[0].isAutoIncrement);
    EXPECT_FALSE(createStmt->columns[0].isUnique);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, "VARCHAR");
    EXPECT_TRUE(createStmt->columns[1].isUnique);
    EXPECT_FALSE(createStmt->columns[1].isAutoIncrement);
}

TEST_F(ParserTest, ComplexParsingScenario) {
    {
        auto stmt = Parser::parse(
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
            ");"
        );
        auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
        ASSERT_NE(createStmt, nullptr);
        EXPECT_EQ(createStmt->tableName, "Employees");
        ASSERT_EQ(createStmt->columns.size(), 9);
        
        std::vector<std::pair<std::string, std::string>> expectedColumns = {
            {"ID", "INT"},
            {"FirstName", "VARCHAR"},
            {"LastName", "VARCHAR"},
            {"Age", "INT"},
            {"Salary", "DOUBLE"},
            {"IsManager", "BOOL"},
            {"IsFullTime", "BOOL"},
            {"YearsOfService", "DOUBLE"},
            {"PerformanceScore", "INT"}
        };

        for (size_t i = 0; i < expectedColumns.size(); ++i) {
            EXPECT_EQ(createStmt->columns[i].name, expectedColumns[i].first);
            EXPECT_EQ(createStmt->columns[i].type, expectedColumns[i].second);
        }
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> insertTests = {
        {
            "INSERT INTO Employees VALUES (1, \"John\", \"Doe\", 30, 50000.50, true, true, 5.5, 95);",
            {"1", "\"John\"", "\"Doe\"", "30", "50000.50", "true", "true", "5.5", "95"}
        },
        
        {
            "INSERT INTO Employees VALUES (2, \"Jane\", \"Smith\", 25 + 3, 60000.0 * 1.1, false, true, (2.5 + 1.5) * 2, 100 - 10);",
            {"2", "\"Jane\"", "\"Smith\"", "25 + 3", "60000.0 * 1.1", "false", "true", "(2.5 + 1.5) * 2", "100 - 10"}
        },
        
        {
            "INSERT INTO Employees VALUES (3, \"Bob\", \"Johnson\", ((40 - 5) + 2) * 1, 45000.0 + (1000.0 * 12), true && false, true || false, ((1.5 + 2.5) * 2) / 2, (85 + 5) * 1);",
            {"3", "\"Bob\"", "\"Johnson\"", "((40 - 5) + 2) * 1", "45000.0 + (1000.0 * 12)", "true && false", "true || false", "((1.5 + 2.5) * 2) / 2", "(85 + 5) * 1"}
        },
        
        {
            "INSERT INTO Employees VALUES (4, \"Alice\", \"Williams\", 20 + (2 * 5), 55000.0, true && (false || true), (true && true) || false, 3.5, (90 + 5) - (10 / 2));",
            {"4", "\"Alice\"", "\"Williams\"", "20 + (2 * 5)", "55000.0", "true && (false || true)", "(true && true) || false", "3.5", "(90 + 5) - (10 / 2)"}
        }
    };

    for (const auto& test : insertTests) {
        auto stmt = Parser::parse(test.first);
        auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
        ASSERT_NE(insertStmt, nullptr);
        EXPECT_EQ(insertStmt->tableName, "Employees");
        ASSERT_EQ(insertStmt->values.size(), test.second.size());
        
        for (size_t i = 0; i < test.second.size(); ++i) {
            EXPECT_EQ(insertStmt->values[i], test.second[i]) 
                << "Mismatch at position " << i << " in query: " << test.first;
        }
    }
}

TEST_F(ParserTest, InvalidSyntax) {
    EXPECT_THROW(Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR)"), std::runtime_error);

    EXPECT_THROW(Parser::parse("TABLE CREATE Test (ID INT);"), std::runtime_error);

    EXPECT_THROW(Parser::parse("CREATE TABLE Test ID INT, Name VARCHAR;"), std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES (1, \"Alice);"), std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test (1, \"Alice\");"), std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES ((1, \"Alice\");"), std::runtime_error);
}

TEST_F(ParserTest, ParseInsertWithColumnAssignments) {
    auto stmt = Parser::parse("INSERT INTO Employees (ID = 1, FirstName = \"John\", LastName = \"Doe\", Age = 30, Salary = 50000.50, IsManager = true && false, IsFullTime = true, YearsOfService = 5.5, PerformanceScore = 95);");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->tableName, "Employees");
    ASSERT_EQ(insertStmt->columnValuePairs.size(), 9);

    EXPECT_EQ(insertStmt->columnValuePairs["ID"], "1");
    EXPECT_EQ(insertStmt->columnValuePairs["FirstName"], "\"John\"");
    EXPECT_EQ(insertStmt->columnValuePairs["LastName"], "\"Doe\"");
    EXPECT_EQ(insertStmt->columnValuePairs["Age"], "30");
    EXPECT_EQ(insertStmt->columnValuePairs["Salary"], "50000.50");
    EXPECT_EQ(insertStmt->columnValuePairs["IsManager"], "true && false");
    EXPECT_EQ(insertStmt->columnValuePairs["IsFullTime"], "true");
    EXPECT_EQ(insertStmt->columnValuePairs["YearsOfService"], "5.5");
    EXPECT_EQ(insertStmt->columnValuePairs["PerformanceScore"], "95");
}

TEST_F(ParserTest, ParseInsertWithColumnAssignmentsAndExpressions) {
    auto stmt = Parser::parse("INSERT INTO Employees (ID = 2, FirstName = \"Jane\", LastName = \"Smith\", Age = 25 + 3, Salary = 60000.0 * 1.1, IsManager = false, IsFullTime = true, YearsOfService = (2.5 + 1.5) * 2, PerformanceScore = 100 - 10);");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->tableName, "Employees");
    ASSERT_EQ(insertStmt->columnValuePairs.size(), 9);

    EXPECT_EQ(insertStmt->columnValuePairs["ID"], "2");
    EXPECT_EQ(insertStmt->columnValuePairs["FirstName"], "\"Jane\"");
    EXPECT_EQ(insertStmt->columnValuePairs["LastName"], "\"Smith\"");
    EXPECT_EQ(insertStmt->columnValuePairs["Age"], "25 + 3");
    EXPECT_EQ(insertStmt->columnValuePairs["Salary"], "60000.0 * 1.1");
    EXPECT_EQ(insertStmt->columnValuePairs["IsManager"], "false");
    EXPECT_EQ(insertStmt->columnValuePairs["IsFullTime"], "true");
    EXPECT_EQ(insertStmt->columnValuePairs["YearsOfService"], "(2.5 + 1.5) * 2");
    EXPECT_EQ(insertStmt->columnValuePairs["PerformanceScore"], "100 - 10");
}

TEST_F(ParserTest, ParseInsertMixedSyntax) {
    {
        auto stmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
        auto insertStmt1 = dynamic_cast<InsertStatement*>(stmt1.get());
        ASSERT_NE(insertStmt1, nullptr);
        EXPECT_EQ(insertStmt1->tableName, "Test");
        ASSERT_EQ(insertStmt1->values.size(), 2);
        EXPECT_EQ(insertStmt1->values[0], "1");
        EXPECT_EQ(insertStmt1->values[1], "\"Alice\"");
    }

    {
        auto stmt2 = Parser::parse("INSERT INTO Test (ID = 2, Name = \"Bob\");");
        auto insertStmt2 = dynamic_cast<InsertStatement*>(stmt2.get());
        ASSERT_NE(insertStmt2, nullptr);
        EXPECT_EQ(insertStmt2->tableName, "Test");
        ASSERT_EQ(insertStmt2->columnValuePairs.size(), 2);
        EXPECT_EQ(insertStmt2->columnValuePairs["ID"], "2");
        EXPECT_EQ(insertStmt2->columnValuePairs["Name"], "\"Bob\"");
    }
}

TEST_F(ParserTest, InvalidSyntaxWithColumnAssignments) {
    EXPECT_THROW(Parser::parse("INSERT INTO Test (ID 1, Name = \"Alice\");"), std::runtime_error);
}

TEST_F(ParserTest, ParseCreateTableWithDefault) {
    auto stmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Name VARCHAR DEFAULT \"Unknown\","
        "    Age INT DEFAULT 18,"
        "    Active BOOL DEFAULT true,"
        "    Salary DOUBLE DEFAULT 1000.0"
        ");"
    );
    
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 5);

    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, "INT");
    EXPECT_FALSE(createStmt->columns[0].hasDefault);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, "VARCHAR");
    EXPECT_TRUE(createStmt->columns[1].hasDefault);
    EXPECT_EQ(createStmt->columns[1].defaultValue, "\"Unknown\"");

    EXPECT_EQ(createStmt->columns[2].name, "Age");
    EXPECT_EQ(createStmt->columns[2].type, "INT");
    EXPECT_TRUE(createStmt->columns[2].hasDefault);
    EXPECT_EQ(createStmt->columns[2].defaultValue, "18");

    EXPECT_EQ(createStmt->columns[3].name, "Active");
    EXPECT_EQ(createStmt->columns[3].type, "BOOL");
    EXPECT_TRUE(createStmt->columns[3].hasDefault);
    EXPECT_EQ(createStmt->columns[3].defaultValue, "true");

    EXPECT_EQ(createStmt->columns[4].name, "Salary");
    EXPECT_EQ(createStmt->columns[4].type, "DOUBLE");
    EXPECT_TRUE(createStmt->columns[4].hasDefault);
    EXPECT_EQ(createStmt->columns[4].defaultValue, "1000.0");
}

TEST_F(ParserTest, ParseCreateTableWithDefaultExpressions) {
    auto stmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Score INT DEFAULT 10 * 5,"
        "    IsValid BOOL DEFAULT true && false"
        ");"
    );
    
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->columns[1].defaultValue, "10 * 5");
    EXPECT_EQ(createStmt->columns[2].defaultValue, "true && false");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 