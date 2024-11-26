#include <gtest/gtest.h>

#include "Parser.h"

using namespace database;

class ParserTest : public ::testing::Test {};

TEST_F(ParserTest, ParseCreateTable) {
    auto stmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 2);
    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, DataTypeName::INT);
    EXPECT_FALSE(createStmt->columns[0].isUnique);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, DataTypeName::STRING);
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
    auto stmt = Parser::parse(
        "CREATE TABLE Test (ID INT AUTOINCREMENT, Name VARCHAR UNIQUE);");
    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 2);

    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, DataTypeName::INT);
    EXPECT_TRUE(createStmt->columns[0].isAutoIncrement);
    EXPECT_FALSE(createStmt->columns[0].isUnique);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, DataTypeName::STRING);
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
            ");");
        auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
        ASSERT_NE(createStmt, nullptr);
        EXPECT_EQ(createStmt->tableName, "Employees");
        ASSERT_EQ(createStmt->columns.size(), 9);

        std::vector<std::pair<std::string, DataTypeName>> expectedColumns = {
            {"ID", DataTypeName::INT},
            {"FirstName", DataTypeName::STRING},
            {"LastName", DataTypeName::STRING},
            {"Age", DataTypeName::INT},
            {"Salary", DataTypeName::DOUBLE },
            {"IsManager", DataTypeName::BOOL},
            {"IsFullTime", DataTypeName::BOOL},
            {"YearsOfService", DataTypeName::DOUBLE},
            {"PerformanceScore", DataTypeName::INT}};

        for (size_t i = 0; i < expectedColumns.size(); ++i) {
            EXPECT_EQ(createStmt->columns[i].name, expectedColumns[i].first);
            EXPECT_EQ(createStmt->columns[i].type, expectedColumns[i].second);
        }
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> insertTests =
        {{"INSERT INTO Employees VALUES (1, \"John\", \"Doe\", 30, 50000.50, "
          "true, true, 5.5, 95);",
          {"1", "\"John\"", "\"Doe\"", "30", "50000.50", "true", "true", "5.5",
           "95"}},

         {"INSERT INTO Employees VALUES (2, \"Jane\", \"Smith\", 25 + 3, "
          "60000.0 * 1.1, false, true, (2.5 + 1.5) * 2, 100 - 10);",
          {"2", "\"Jane\"", "\"Smith\"", "25 + 3", "60000.0 * 1.1", "false",
           "true", "(2.5 + 1.5) * 2", "100 - 10"}},

         {"INSERT INTO Employees VALUES (3, \"Bob\", \"Johnson\", ((40 - 5) + "
          "2) * 1, 45000.0 + (1000.0 * 12), true && false, true || false, "
          "((1.5 + 2.5) * 2) / 2, (85 + 5) * 1);",
          {"3", "\"Bob\"", "\"Johnson\"", "((40 - 5) + 2) * 1",
           "45000.0 + (1000.0 * 12)", "true && false", "true || false",
           "((1.5 + 2.5) * 2) / 2", "(85 + 5) * 1"}},

         {"INSERT INTO Employees VALUES (4, \"Alice\", \"Williams\", 20 + (2 * "
          "5), 55000.0, true && (false || true), (true && true) || false, 3.5, "
          "(90 + 5) - (10 / 2));",
          {"4", "\"Alice\"", "\"Williams\"", "20 + (2 * 5)", "55000.0",
           "true && (false || true)", "(true && true) || false", "3.5",
           "(90 + 5) - (10 / 2)"}}};

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
    EXPECT_THROW(Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR)"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("TABLE CREATE Test (ID INT);"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("CREATE TABLE Test ID INT, Name VARCHAR;"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES (1, \"Alice);"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test (1, \"Alice\");"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES ((1, \"Alice\");"),
                 std::runtime_error);
}

TEST_F(ParserTest, ParseInsertWithColumnAssignments) {
    auto stmt = Parser::parse(
        "INSERT INTO Employees (ID = 1, FirstName = \"John\", LastName = "
        "\"Doe\", Age = 30, Salary = 50000.50, IsManager = true && false, "
        "IsFullTime = true, YearsOfService = 5.5, PerformanceScore = 95);");
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
    auto stmt = Parser::parse(
        "INSERT INTO Employees (ID = 2, FirstName = \"Jane\", LastName = "
        "\"Smith\", Age = 25 + 3, Salary = 60000.0 * 1.1, IsManager = false, "
        "IsFullTime = true, YearsOfService = (2.5 + 1.5) * 2, PerformanceScore "
        "= 100 - 10);");
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
    EXPECT_EQ(insertStmt->columnValuePairs["YearsOfService"],
              "(2.5 + 1.5) * 2");
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
        auto stmt2 =
            Parser::parse("INSERT INTO Test (ID = 2, Name = \"Bob\");");
        auto insertStmt2 = dynamic_cast<InsertStatement*>(stmt2.get());
        ASSERT_NE(insertStmt2, nullptr);
        EXPECT_EQ(insertStmt2->tableName, "Test");
        ASSERT_EQ(insertStmt2->columnValuePairs.size(), 2);
        EXPECT_EQ(insertStmt2->columnValuePairs["ID"], "2");
        EXPECT_EQ(insertStmt2->columnValuePairs["Name"], "\"Bob\"");
    }
}

TEST_F(ParserTest, InvalidSyntaxWithColumnAssignments) {
    EXPECT_THROW(Parser::parse("INSERT INTO Test (ID 1, Name = \"Alice\");"),
                 std::runtime_error);
}

TEST_F(ParserTest, ParseCreateTableWithDefault) {
    auto stmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Name VARCHAR DEFAULT \"Unknown\","
        "    Age INT DEFAULT 18,"
        "    Active BOOL DEFAULT true,"
        "    Salary DOUBLE DEFAULT 1000.0"
        ");");

    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->tableName, "Test");
    ASSERT_EQ(createStmt->columns.size(), 5);

    EXPECT_EQ(createStmt->columns[0].name, "ID");
    EXPECT_EQ(createStmt->columns[0].type, DataTypeName::INT);
    EXPECT_FALSE(createStmt->columns[0].hasDefault);

    EXPECT_EQ(createStmt->columns[1].name, "Name");
    EXPECT_EQ(createStmt->columns[1].type, DataTypeName::STRING);
    EXPECT_TRUE(createStmt->columns[1].hasDefault);
    EXPECT_EQ(createStmt->columns[1].defaultValue, "\"Unknown\"");

    EXPECT_EQ(createStmt->columns[2].name, "Age");
    EXPECT_EQ(createStmt->columns[2].type, DataTypeName::INT);
    EXPECT_TRUE(createStmt->columns[2].hasDefault);
    EXPECT_EQ(createStmt->columns[2].defaultValue, "18");

    EXPECT_EQ(createStmt->columns[3].name, "Active");
    EXPECT_EQ(createStmt->columns[3].type, DataTypeName::BOOL);
    EXPECT_TRUE(createStmt->columns[3].hasDefault);
    EXPECT_EQ(createStmt->columns[3].defaultValue, "true");

    EXPECT_EQ(createStmt->columns[4].name, "Salary");
    EXPECT_EQ(createStmt->columns[4].type, DataTypeName::DOUBLE);
    EXPECT_TRUE(createStmt->columns[4].hasDefault);
    EXPECT_EQ(createStmt->columns[4].defaultValue, "1000.0");
}

TEST_F(ParserTest, ParseCreateTableWithDefaultExpressions) {
    auto stmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Score INT DEFAULT 10 * 5,"
        "    IsValid BOOL DEFAULT true && false"
        ");");

    auto createStmt = dynamic_cast<CreateTableStatement*>(stmt.get());
    ASSERT_NE(createStmt, nullptr);
    EXPECT_EQ(createStmt->columns[1].defaultValue, "10 * 5");
    EXPECT_EQ(createStmt->columns[2].defaultValue, "true && false");
}

TEST_F(ParserTest, ParseInsertWithEmptyValues) {
    auto stmt =
        Parser::parse("INSERT INTO Test VALUES (1, , \"Alice\", , 25);");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->tableName, "Test");
    ASSERT_EQ(insertStmt->values.size(), 5);
    EXPECT_EQ(insertStmt->values[0], "1");
    EXPECT_EQ(insertStmt->values[1], "");
    EXPECT_EQ(insertStmt->values[2], "\"Alice\"");
    EXPECT_EQ(insertStmt->values[3], "");
    EXPECT_EQ(insertStmt->values[4], "25");
}

TEST_F(ParserTest, ParseInsertWithEmptyValuesTwo) {
    auto stmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, , \"Alice\", , 25);");
    auto insertStmt1 = dynamic_cast<InsertStatement*>(stmt1.get());
    ASSERT_NE(insertStmt1, nullptr);
    EXPECT_EQ(insertStmt1->values.size(), 5);
    EXPECT_EQ(insertStmt1->values[0], "1");
    EXPECT_EQ(insertStmt1->values[1], "");
    EXPECT_EQ(insertStmt1->values[2], "\"Alice\"");
    EXPECT_EQ(insertStmt1->values[3], "");
    EXPECT_EQ(insertStmt1->values[4], "25");

    auto stmt2 = Parser::parse("INSERT INTO Test VALUES (, 1, \"Bob\", 30, );");
    auto insertStmt2 = dynamic_cast<InsertStatement*>(stmt2.get());
    ASSERT_NE(insertStmt2, nullptr);
    EXPECT_EQ(insertStmt2->values.size(), 5);
    EXPECT_EQ(insertStmt2->values[0], "");
    EXPECT_EQ(insertStmt2->values[4], "");

    auto stmt3 = Parser::parse("INSERT INTO Test VALUES (1, , , , 5);");
    auto insertStmt3 = dynamic_cast<InsertStatement*>(stmt3.get());
    ASSERT_NE(insertStmt3, nullptr);
    EXPECT_EQ(insertStmt3->values.size(), 5);
    EXPECT_EQ(insertStmt3->values[1], "");
    EXPECT_EQ(insertStmt3->values[2], "");
    EXPECT_EQ(insertStmt3->values[3], "");

    auto stmt4 = Parser::parse("INSERT INTO Test VALUES (, , , );");
    auto insertStmt4 = dynamic_cast<InsertStatement*>(stmt4.get());
    ASSERT_NE(insertStmt4, nullptr);
    EXPECT_EQ(insertStmt4->values.size(), 4);
    for (const auto& value : insertStmt4->values) {
        EXPECT_EQ(value, "");
    }
}

TEST_F(ParserTest, ParseInsertWithComplexExpressions) {
    auto stmt = Parser::parse(
        "INSERT INTO Test VALUES ("
        "1 + 2 * 3, "
        "(4 + 5) * (6 + 7), "
        "\"Complex \\\"String\\\" Here\", "
        "true && (false || true), "
        "2.5 * (3.0 + 4.5)"
        ");");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->values.size(), 5);
    EXPECT_EQ(insertStmt->values[0], "1 + 2 * 3");
    EXPECT_EQ(insertStmt->values[1], "(4 + 5) * (6 + 7)");
    EXPECT_EQ(insertStmt->values[2], "\"Complex \\\"String\\\" Here\"");
    EXPECT_EQ(insertStmt->values[3], "true && (false || true)");
    EXPECT_EQ(insertStmt->values[4], "2.5 * (3.0 + 4.5)");
}

TEST_F(ParserTest, ParseInsertEdgeCases) {
    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES (1, \"unclosed);"),
                 std::runtime_error);

    EXPECT_THROW(Parser::parse("INSERT INTO Test VALUES (1, (2 + 3);"),
                 std::runtime_error);
}

TEST_F(ParserTest, ParseInsertWithWhitespace) {
    auto stmt = Parser::parse(
        "INSERT   INTO   Test   VALUES   (  1  ,  \"Alice\"  ,  25  )  ;");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->values.size(), 3);
    EXPECT_EQ(insertStmt->values[0], "1");
    EXPECT_EQ(insertStmt->values[1], "\"Alice\"");
    EXPECT_EQ(insertStmt->values[2], "25");
}

TEST_F(ParserTest, ParseInsertWithNewlines) {
    auto stmt = Parser::parse(
        "INSERT INTO Test VALUES (\n"
        "    1,\n"
        "    \"Alice\",\n"
        "    25\n"
        ");");
    auto insertStmt = dynamic_cast<InsertStatement*>(stmt.get());
    ASSERT_NE(insertStmt, nullptr);
    EXPECT_EQ(insertStmt->values.size(), 3);
    EXPECT_EQ(insertStmt->values[0], "1");
    EXPECT_EQ(insertStmt->values[1], "\"Alice\"");
    EXPECT_EQ(insertStmt->values[2], "25");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST_F(ParserTest, ParseSelect) {
    auto stmt = Parser::parse("SELECT * FROM Test;");
    auto selectStmt = dynamic_cast<SelectStatement*>(stmt.get());
    ASSERT_NE(selectStmt, nullptr);
    EXPECT_EQ(selectStmt->tableName, "Test");
    ASSERT_EQ(selectStmt->columnNames.size(), 1);
    EXPECT_EQ(selectStmt->columnNames[0], "*");
    EXPECT_TRUE(selectStmt->predicate.empty());
}

TEST_F(ParserTest, ParseSelectWithColumns) {
    auto stmt = Parser::parse("SELECT ID, Name FROM Test;");
    auto selectStmt = dynamic_cast<SelectStatement*>(stmt.get());
    ASSERT_NE(selectStmt, nullptr);
    EXPECT_EQ(selectStmt->tableName, "Test");
    ASSERT_EQ(selectStmt->columnNames.size(), 2);
    EXPECT_EQ(selectStmt->columnNames[0], "ID");
    EXPECT_EQ(selectStmt->columnNames[1], "Name");
    EXPECT_TRUE(selectStmt->predicate.empty());
}

TEST_F(ParserTest, ParseSelectWithPredicate) {
    auto stmt = Parser::parse("SELECT * FROM Test WHERE ID == 1;");
    auto selectStmt = dynamic_cast<SelectStatement*>(stmt.get());
    ASSERT_NE(selectStmt, nullptr);
    EXPECT_EQ(selectStmt->tableName, "Test");
    ASSERT_EQ(selectStmt->columnNames.size(), 1);
    EXPECT_EQ(selectStmt->columnNames[0], "*");
    EXPECT_EQ(selectStmt->predicate, "ID == 1");
}

TEST_F(ParserTest, ParseSelectWithComplexPredicate) {
    auto stmt = Parser::parse(
        "SELECT * FROM Test WHERE (ID == 1) && (Name == \"Alice\");");
    auto selectStmt = dynamic_cast<SelectStatement*>(stmt.get());
    ASSERT_NE(selectStmt, nullptr);
    EXPECT_EQ(selectStmt->tableName, "Test");
    ASSERT_EQ(selectStmt->columnNames.size(), 1);
    EXPECT_EQ(selectStmt->columnNames[0], "*");
    EXPECT_EQ(selectStmt->predicate, "(ID == 1) && (Name == \"Alice\")");
}

TEST_F(ParserTest, ParseUpdate) {
    auto stmt = Parser::parse(
        "UPDATE Test SET (ID = 1, Name = \"Alice\") WHERE ID == 2;");
    auto updateStmt = dynamic_cast<UpdateStatement*>(stmt.get());
    ASSERT_NE(updateStmt, nullptr);
    EXPECT_EQ(updateStmt->tableName, "Test");
    ASSERT_EQ(updateStmt->newValues.size(), 2);
    EXPECT_EQ(updateStmt->newValues["ID"], "1");
    EXPECT_EQ(updateStmt->newValues["Name"], "\"Alice\"");
    EXPECT_EQ(updateStmt->predicate, "ID == 2");
}

TEST_F(ParserTest, ParseUpdateWithComplexExpressions) {
    auto stmt = Parser::parse(
        "UPDATE Test SET (ID = 1 + 2 * 3, Name = \"Alice\" + \"Smith\") WHERE "
        "ID == 2;");
    auto updateStmt = dynamic_cast<UpdateStatement*>(stmt.get());
    ASSERT_NE(updateStmt, nullptr);
    EXPECT_EQ(updateStmt->tableName, "Test");
    ASSERT_EQ(updateStmt->newValues.size(), 2);
    EXPECT_EQ(updateStmt->newValues["ID"], "1 + 2 * 3");
    EXPECT_EQ(updateStmt->newValues["Name"], "\"Alice\" + \"Smith\"");
    EXPECT_EQ(updateStmt->predicate, "ID == 2");
}

TEST_F(ParserTest, ParseUpdateWithWhitespace) {
    auto stmt = Parser::parse(
        "UPDATE   Test   SET   (  ID  =  1  ,  Name  =  \"Alice\"  )   WHERE   "
        "ID  ==  2  ;");
    auto updateStmt = dynamic_cast<UpdateStatement*>(stmt.get());
    ASSERT_NE(updateStmt, nullptr);
    EXPECT_EQ(updateStmt->tableName, "Test");
    ASSERT_EQ(updateStmt->newValues.size(), 2);
    EXPECT_EQ(updateStmt->newValues["ID"], "1");
    EXPECT_EQ(updateStmt->newValues["Name"], "\"Alice\"");
    EXPECT_EQ(updateStmt->predicate, "ID == 2");
}

TEST_F(ParserTest, ParseUpdateWithNewlines) {
    auto stmt = Parser::parse(
        "UPDATE Test SET (\n"
        "    ID = 1,\n"
        "    Name = \"Alice\"\n"
        ") WHERE ID == 2;");
    auto updateStmt = dynamic_cast<UpdateStatement*>(stmt.get());
    ASSERT_NE(updateStmt, nullptr);
    EXPECT_EQ(updateStmt->tableName, "Test");
    ASSERT_EQ(updateStmt->newValues.size(), 2);
    EXPECT_EQ(updateStmt->newValues["ID"], "1");
    EXPECT_EQ(updateStmt->newValues["Name"], "\"Alice\"");
    EXPECT_EQ(updateStmt->predicate, "ID == 2");
}

TEST_F(ParserTest, ParseDelete) {
    auto stmt = Parser::parse("DELETE FROM Test WHERE ID == 1;");
    auto deleteStmt = dynamic_cast<DeleteStatement*>(stmt.get());
    ASSERT_NE(deleteStmt, nullptr);
    EXPECT_EQ(deleteStmt->tableName, "Test");
    EXPECT_EQ(deleteStmt->predicate, "ID == 1");
}

TEST_F(ParserTest, ParseDeleteWithComplexPredicate) {
    auto stmt = Parser::parse(
        "DELETE FROM Test WHERE (ID == 1) && (Name == \"Alice\");");
    auto deleteStmt = dynamic_cast<DeleteStatement*>(stmt.get());
    ASSERT_NE(deleteStmt, nullptr);
    EXPECT_EQ(deleteStmt->tableName, "Test");
    EXPECT_EQ(deleteStmt->predicate, "(ID == 1) && (Name == \"Alice\")");
}

TEST_F(ParserTest, ParseDeleteWithWhitespace) {
    auto stmt = Parser::parse("DELETE   FROM   Test   WHERE   ID  ==  1  ;");
    auto deleteStmt = dynamic_cast<DeleteStatement*>(stmt.get());
    ASSERT_NE(deleteStmt, nullptr);
    EXPECT_EQ(deleteStmt->tableName, "Test");
    EXPECT_EQ(deleteStmt->predicate, "ID == 1");
}

TEST_F(ParserTest, ParseDeleteWithNewlines) {
    auto stmt = Parser::parse(
        "DELETE FROM Test WHERE\n"
        "    ID == 1;");
    auto deleteStmt = dynamic_cast<DeleteStatement*>(stmt.get());
    ASSERT_NE(deleteStmt, nullptr);
    EXPECT_EQ(deleteStmt->tableName, "Test");
    EXPECT_EQ(deleteStmt->predicate, "ID == 1");
}