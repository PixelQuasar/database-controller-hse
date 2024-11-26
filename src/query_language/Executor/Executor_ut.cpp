#include <gtest/gtest.h>

#include "../../database/Database/Database.h"
#include "../Parser/Parser.h"
#include "Executor.h"

using namespace database;

class ExecutorTest : public ::testing::Test {
   protected:
    Database db;
    Executor executor{db};
};

TEST_F(ExecutorTest, ExecuteCreateTable) {
    auto stmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(stmt);
    const auto& table = db.getTable("Test");
    EXPECT_EQ(table.get_name(), "Test");
    ASSERT_EQ(table.get_scheme().size(), 2);
    EXPECT_EQ(table.get_scheme()[0].name, "ID");
    EXPECT_EQ(table.get_scheme()[1].name, "Name");
}

TEST_F(ExecutorTest, ExecuteCreateTable1) {
    auto stmt = "CREATE TABLE Test (ID INT, Name VARCHAR);";
    executor.execute(stmt);
    const auto& table = db.getTable("Test");
    EXPECT_EQ(table.get_name(), "Test");
    ASSERT_EQ(table.get_scheme().size(), 2);
    EXPECT_EQ(table.get_scheme()[0].name, "ID");
    EXPECT_EQ(table.get_scheme()[1].name, "Name");
}

TEST_F(ExecutorTest, ExecuteInsert) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    executor.execute(insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
}

TEST_F(ExecutorTest, ExecuteInsert1) {
    auto createStmt = "CREATE TABLE Test (ID INT, Name VARCHAR);";
    executor.execute(createStmt);

    auto insertStmt = "INSERT INTO Test VALUES (1, \"Alice\");";
    executor.execute(insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
}

TEST_F(ExecutorTest, ExecuteMultipleInserts) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    executor.execute(insertStmt1);

    auto insertStmt2 = Parser::parse("INSERT INTO Test VALUES (2, \"Bob\");");
    executor.execute(insertStmt2);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 2);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<int>(data[1][0]), 2);
    EXPECT_EQ(std::get<std::string>(data[1][1]), "Bob");
}

TEST_F(ExecutorTest, ExecuteMultipleInserts1) {
    auto createStmt = "CREATE TABLE Test (ID INT, Name VARCHAR);";
    executor.execute(createStmt);

    auto insertStmt1 = "INSERT INTO Test VALUES (1, \"Alice\");";
    executor.execute(insertStmt1);

    auto insertStmt2 = "INSERT INTO Test VALUES (2, \"Bob\");";
    executor.execute(insertStmt2);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 2);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<int>(data[1][0]), 2);
    EXPECT_EQ(std::get<std::string>(data[1][1]), "Bob");
}

TEST_F(ExecutorTest, ExecuteInsertWithExpression) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 20 + 5);");
    executor.execute(insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<int>(data[0][2]), 25);
}

TEST_F(ExecutorTest, ExecuteInsertWithBoolean) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Active BOOL);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", true);");
    executor.execute(insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<bool>(data[0][2]), true);
}

TEST_F(ExecutorTest, ExecuteInsertWithInvalidType) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    auto result1 = executor.execute(createStmt);
    EXPECT_TRUE(result1.is_ok());
    auto insertStmt = Parser::parse("INSERT INTO Test VALUES (\"Alice\", 1);");
    auto result2 = executor.execute(insertStmt);
    EXPECT_FALSE(result2.is_ok());
}

TEST_F(ExecutorTest, ExecuteCreateTableDuplicate) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    executor.execute(createStmt);
    auto duplicateStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR);");
    auto result = executor.execute(duplicateStmt);
    EXPECT_FALSE(result.is_ok());
}

TEST_F(ExecutorTest, ExecuteInsertIntoNonExistentTable) {
    auto insertStmt =
        Parser::parse("INSERT INTO NonExistent VALUES (1, \"Alice\");");
    auto result = executor.execute(insertStmt);
    EXPECT_FALSE(result.is_ok());
}

TEST_F(ExecutorTest, ExecuteComplexInsert) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test (ID INT, Name VARCHAR, Age INT, Active BOOL);");
    executor.execute(createStmt);

    auto insertStmt = Parser::parse(
        "INSERT INTO Test VALUES (1, \"Alice\", 20 + 5, true && false);");
    executor.execute(insertStmt);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<int>(data[0][2]), 25);
    EXPECT_EQ(std::get<bool>(data[0][3]), false);
}

TEST_F(ExecutorTest, ComplexScenario) {
    auto createStmt = Parser::parse(
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

    executor.execute(createStmt);

    const auto& table = db.getTable("Employees");
    EXPECT_EQ(table.get_name(), "Employees");
    ASSERT_EQ(table.get_scheme().size(), 9);

    std::vector<std::string> insertStatements = {
        "INSERT INTO Employees VALUES (1, \"John\", \"Doe\", 30, 50000.50, "
        "true, true, 5.5, 95);",

        "INSERT INTO Employees VALUES (2, \"Jane\", \"Smith\", 25 + 3, 60000.0 "
        "* 1.1, false, true, (2.5 + 1.5) * 2, 100 - 10);",

        "INSERT INTO Employees VALUES (3, \"Bob\", \"Johnson\", ((40 - 5) + 2) "
        "* 1, 45000.0 + (1000.0 * 12), true && false, true || false, ((1.5 + "
        "2.5) * 2) / 2, (85 + 5) * 1);",

        "INSERT INTO Employees VALUES (4, \"Alice\", \"Williams\", 20 + (2 * "
        "5), 55000.0, true && (false || true), (true && true) || false, 3.5, "
        "(90 + 5) - (10 / 2));"};

    for (const auto& sql : insertStatements) {
        auto stmt = Parser::parse(sql);
        executor.execute(stmt);
    }

    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 4);

    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "John");
    EXPECT_EQ(std::get<std::string>(data[0][2]), "Doe");
    EXPECT_EQ(std::get<int>(data[0][3]), 30);
    EXPECT_DOUBLE_EQ(std::get<double>(data[0][4]), 50000.50);
    EXPECT_EQ(std::get<bool>(data[0][5]), true);
    EXPECT_EQ(std::get<bool>(data[0][6]), true);
    EXPECT_DOUBLE_EQ(std::get<double>(data[0][7]), 5.5);
    EXPECT_EQ(std::get<int>(data[0][8]), 95);

    EXPECT_EQ(std::get<int>(data[1][0]), 2);
    EXPECT_EQ(std::get<std::string>(data[1][1]), "Jane");
    EXPECT_EQ(std::get<std::string>(data[1][2]), "Smith");
    EXPECT_EQ(std::get<int>(data[1][3]), 28);
    EXPECT_DOUBLE_EQ(std::get<double>(data[1][4]), 66000.0);
    EXPECT_EQ(std::get<bool>(data[1][5]), false);
    EXPECT_EQ(std::get<bool>(data[1][6]), true);
    EXPECT_DOUBLE_EQ(std::get<double>(data[1][7]), 8.0);
    EXPECT_EQ(std::get<int>(data[1][8]), 90);

    EXPECT_EQ(std::get<int>(data[2][0]), 3);
    EXPECT_EQ(std::get<std::string>(data[2][1]), "Bob");
    EXPECT_EQ(std::get<std::string>(data[2][2]), "Johnson");
    EXPECT_EQ(std::get<int>(data[2][3]), 37);
    EXPECT_DOUBLE_EQ(std::get<double>(data[2][4]), 57000.0);
    EXPECT_EQ(std::get<bool>(data[2][5]), false);
    EXPECT_EQ(std::get<bool>(data[2][6]), true);
    EXPECT_DOUBLE_EQ(std::get<double>(data[2][7]), 4.0);
    EXPECT_EQ(std::get<int>(data[2][8]), 90);

    EXPECT_EQ(std::get<int>(data[3][0]), 4);
    EXPECT_EQ(std::get<std::string>(data[3][1]), "Alice");
    EXPECT_EQ(std::get<std::string>(data[3][2]), "Williams");
    EXPECT_EQ(std::get<int>(data[3][3]), 30);
    EXPECT_DOUBLE_EQ(std::get<double>(data[3][4]), 55000.0);
    EXPECT_EQ(std::get<bool>(data[3][5]), true);
    EXPECT_EQ(std::get<bool>(data[3][6]), true);
    EXPECT_DOUBLE_EQ(std::get<double>(data[3][7]), 3.5);
    EXPECT_EQ(std::get<int>(data[3][8]), 90);
}

TEST_F(ExecutorTest, UniqueConstraint) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT UNIQUE, Name VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    executor.execute(insertStmt1);

    auto insertStmt2 = Parser::parse("INSERT INTO Test VALUES (1, \"Bob\");");
    auto result = executor.execute(insertStmt2);
    EXPECT_FALSE(result.is_ok());

    auto insertStmt3 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Charlie\");");
    auto result2 = executor.execute(insertStmt3);
    EXPECT_TRUE(result2.is_ok());

    auto insertStmt4 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Charlie\");");
    auto result3 = executor.execute(insertStmt4);
    EXPECT_FALSE(result3.is_ok());
}

TEST_F(ExecutorTest, AutoIncrement) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test (ID INT AUTOINCREMENT, Name VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (NULL, \"Alice\");");
    executor.execute(insertStmt1);

    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (NULL, \"Bob\");");
    executor.execute(insertStmt2);

    auto insertStmt3 =
        Parser::parse("INSERT INTO Test VALUES (NULL, \"Charlie\");");
    executor.execute(insertStmt3);

    auto insertStmt4 =
        Parser::parse("INSERT INTO Test VALUES (10, \"David\");");
    executor.execute(insertStmt4);

    auto insertStmt5 =
        Parser::parse("INSERT INTO Test VALUES (NULL, \"Eve\");");
    executor.execute(insertStmt5);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 5);
    EXPECT_EQ(std::get<int>(data[0][0]), 0);
    EXPECT_EQ(std::get<int>(data[1][0]), 1);
    EXPECT_EQ(std::get<int>(data[2][0]), 2);
    EXPECT_EQ(std::get<int>(data[3][0]), 10);
    EXPECT_EQ(std::get<int>(data[4][0]), 11);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<std::string>(data[1][1]), "Bob");
    EXPECT_EQ(std::get<std::string>(data[2][1]), "Charlie");
    EXPECT_EQ(std::get<std::string>(data[3][1]), "David");
    EXPECT_EQ(std::get<std::string>(data[4][1]), "Eve");
}

TEST_F(ExecutorTest, MultipleAutoIncrement) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test (ID1 INT AUTOINCREMENT, ID2 INT AUTOINCREMENT, Name "
        "VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (NULL, NULL, \"Alice\");");
    executor.execute(insertStmt1);

    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (NULL, NULL, \"Bob\");");
    executor.execute(insertStmt2);

    auto insertStmt3 =
        Parser::parse("INSERT INTO Test VALUES (NULL, NULL, \"Charlie\");");
    executor.execute(insertStmt3);

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 3);
    EXPECT_EQ(std::get<int>(data[0][0]), 0);
    EXPECT_EQ(std::get<int>(data[0][1]), 0);
    EXPECT_EQ(std::get<int>(data[1][0]), 1);
    EXPECT_EQ(std::get<int>(data[1][1]), 1);
    EXPECT_EQ(std::get<int>(data[2][0]), 2);
    EXPECT_EQ(std::get<int>(data[2][1]), 2);
    EXPECT_EQ(std::get<std::string>(data[0][2]), "Alice");
    EXPECT_EQ(std::get<std::string>(data[1][2]), "Bob");
    EXPECT_EQ(std::get<std::string>(data[2][2]), "Charlie");
}

TEST_F(ExecutorTest, KeyConstraint) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT KEY, Name VARCHAR);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\");");
    executor.execute(insertStmt1);

    auto insertStmt2 = Parser::parse("INSERT INTO Test VALUES (1, \"Bob\");");
    auto result = executor.execute(insertStmt2);
    EXPECT_FALSE(result.is_ok());

    auto insertStmt3 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Charlie\");");
    auto result2 = executor.execute(insertStmt3);
    EXPECT_TRUE(result2.is_ok());
}


TEST_F(ExecutorTest, ExecuteInsertWithAssignments) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test (ID INT, Name VARCHAR, Age INT, Active BOOL);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse(
        "INSERT INTO Test (ID = 1, Name = \"Alice\", Age = 25, Active = "
        "true);");
    auto result1 = executor.execute(insertStmt1);
    EXPECT_TRUE(result1.is_ok());

    {
        const auto& table = db.getTable("Test");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
        EXPECT_EQ(std::get<int>(data[0][0]), 1);
        EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
        EXPECT_EQ(std::get<int>(data[0][2]), 25);
        EXPECT_EQ(std::get<bool>(data[0][3]), true);
    }

    auto insertStmt2 = Parser::parse(
        "INSERT INTO Test (ID = 2 + 1, Name = \"Bob\", Age = 20 * 2, Active = "
        "true && false);");
    auto result2 = executor.execute(insertStmt2);
    EXPECT_TRUE(result2.is_ok());

    {
        const auto& table = db.getTable("Test");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 2);
        EXPECT_EQ(std::get<int>(data[1][0]), 3);
        EXPECT_EQ(std::get<std::string>(data[1][1]), "Bob");
        EXPECT_EQ(std::get<int>(data[1][2]), 40);
        EXPECT_EQ(std::get<bool>(data[1][3]), false);
    }
}

TEST_F(ExecutorTest, ExecuteInvalidInsertWithAssignments) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse(
        "INSERT INTO Test (ID = \"wrong\", Name = 123, Age = true);");
    auto result1 = executor.execute(insertStmt1);
    EXPECT_FALSE(result1.is_ok());

    auto insertStmt2 = Parser::parse(
        "INSERT INTO Test (ID = 1, Name = \"Alice\", Wrong = 25);");
    auto result2 = executor.execute(insertStmt2);
    EXPECT_FALSE(result2.is_ok());

    auto insertStmt3 =
        Parser::parse("INSERT INTO Test (ID = 1, Name = \"Alice\", ID = 2);");
    auto result3 = executor.execute(insertStmt3);
    EXPECT_FALSE(result3.is_ok());

    auto insertStmt4 =
        Parser::parse("INSERT INTO Test (ID = 1, Name = \"Alice\");");
    auto result4 = executor.execute(insertStmt4);
    EXPECT_FALSE(result4.is_ok());

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 0);
}

TEST_F(ExecutorTest, ExecuteInsertWithPartialAssignments) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Name VARCHAR DEFAULT \"Unknown\","
        "    Age INT DEFAULT 18,"
        "    Active BOOL DEFAULT true"
        ");");
    executor.execute(createStmt);

    auto insertStmt = Parser::parse("INSERT INTO Test (ID = 1, Age = 25);");
    auto result = executor.execute(insertStmt);
    EXPECT_TRUE(result.is_ok());

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<int>(data[0][2]), 25);
    EXPECT_EQ(std::get<bool>(data[0][3]), true);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Unknown");
}

TEST_F(ExecutorTest, ExecuteInsertWithMixedSyntax) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto result1 = executor.execute(insertStmt1);
    EXPECT_TRUE(result1.is_ok());

    auto insertStmt2 =
        Parser::parse("INSERT INTO Test (ID = 2, Name = \"Bob\", Age = 30);");
    auto result2 = executor.execute(insertStmt2);
    EXPECT_TRUE(result2.is_ok());

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 2);

    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
    EXPECT_EQ(std::get<int>(data[0][2]), 25);

    EXPECT_EQ(std::get<int>(data[1][0]), 2);
    EXPECT_EQ(std::get<std::string>(data[1][1]), "Bob");
    EXPECT_EQ(std::get<int>(data[1][2]), 30);
}

TEST_F(ExecutorTest, ExecuteInsertWithDefaults) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Name VARCHAR DEFAULT \"Unknown\","
        "    Age INT DEFAULT 18,"
        "    Active BOOL DEFAULT true"
        ");");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test (ID = 1);");
    auto result1 = executor.execute(insertStmt1);
    EXPECT_TRUE(result1.is_ok());

    {
        const auto& table = db.getTable("Test");
        const auto& data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
        EXPECT_EQ(std::get<int>(data[0][0]), 1);
        EXPECT_EQ(std::get<std::string>(data[0][1]), "Unknown");
        EXPECT_EQ(std::get<int>(data[0][2]), 18);
        EXPECT_EQ(std::get<bool>(data[0][3]), true);
    }

    auto insertStmt2 = Parser::parse(
        "INSERT INTO Test (ID = 2, Name = \"John\", Age = 25, Active = "
        "false);");

    auto result2 = executor.execute(insertStmt2);
    EXPECT_TRUE(result2.is_ok());
    {
        const auto& table = db.getTable("Test");
        const auto& data = table.get_rows();
        ASSERT_EQ(data.size(), 2);
        EXPECT_EQ(std::get<int>(data[1][0]), 2);
        EXPECT_EQ(std::get<std::string>(data[1][1]), "John");
        EXPECT_EQ(std::get<int>(data[1][2]), 25);
        EXPECT_EQ(std::get<bool>(data[1][3]), false);
    }
}

TEST_F(ExecutorTest, ComplexTableOperations) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Employees ("
        "    ID INT AUTOINCREMENT KEY,"
        "    EmpCode INT UNIQUE,"
        "    FirstName VARCHAR DEFAULT \"New\","
        "    LastName VARCHAR,"
        "    Age INT DEFAULT 18,"
        "    Salary DOUBLE DEFAULT 1000.0,"
        "    IsActive BOOL DEFAULT true"
        ");");
    auto result = executor.execute(createStmt);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt1 = Parser::parse(
        "INSERT INTO Employees (EmpCode = 101, LastName = \"Doe\");");
    result = executor.execute(insertStmt1);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
        EXPECT_EQ(std::get<int>(data[0][0]), 0);
        EXPECT_EQ(std::get<int>(data[0][1]), 101);
        EXPECT_EQ(std::get<std::string>(data[0][2]), "New");
        EXPECT_EQ(std::get<std::string>(data[0][3]), "Doe");
        EXPECT_EQ(std::get<int>(data[0][4]), 18);
        EXPECT_EQ(std::get<double>(data[0][5]), 1000.0);
        EXPECT_EQ(std::get<bool>(data[0][6]), true);
    }

    auto insertStmt2 = Parser::parse(
        "INSERT INTO Employees ("
        "    EmpCode = 102, "
        "    FirstName = \"John\", "
        "    LastName = \"Smith\", "
        "    Age = 25, "
        "    Salary = 2000.0, "
        "    IsActive = false"
        ");");
    result = executor.execute(insertStmt2);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt3 = Parser::parse(
        "INSERT INTO Employees ("
        "    EmpCode = 100 + 3, "
        "    FirstName = \"Bob\", "
        "    LastName = \"Johnson\", "
        "    Age = 20 + 5, "
        "    Salary = 1500.0 * 2, "
        "    IsActive = true && true"
        ");");
    result = executor.execute(insertStmt3);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 3);

        EXPECT_EQ(std::get<int>(data[1][0]), 1);
        EXPECT_EQ(std::get<int>(data[1][1]), 102);
        EXPECT_EQ(std::get<std::string>(data[1][2]), "John");
        EXPECT_EQ(std::get<std::string>(data[1][3]), "Smith");
        EXPECT_EQ(std::get<int>(data[1][4]), 25);
        EXPECT_EQ(std::get<double>(data[1][5]), 2000.0);
        EXPECT_EQ(std::get<bool>(data[1][6]), false);

        EXPECT_EQ(std::get<int>(data[2][0]), 2);
        EXPECT_EQ(std::get<int>(data[2][1]), 103);
        EXPECT_EQ(std::get<std::string>(data[2][2]), "Bob");
        EXPECT_EQ(std::get<std::string>(data[2][3]), "Johnson");
        EXPECT_EQ(std::get<int>(data[2][4]), 25);
        EXPECT_EQ(std::get<double>(data[2][5]), 3000.0);
        EXPECT_EQ(std::get<bool>(data[2][6]), true);
    }

    auto insertStmt4 = Parser::parse(
        "INSERT INTO Employees (EmpCode = 101, LastName = \"Wilson\");");
    result = executor.execute(insertStmt4);
    EXPECT_FALSE(result.is_ok());

    auto insertStmt5 = Parser::parse(
        "INSERT INTO Employees (ID = 100, EmpCode = 104, LastName = "
        "\"Brown\");");
    result = executor.execute(insertStmt5);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 4);
    }
}

TEST_F(ExecutorTest, ComplexTableOperationsWithValues) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Employees ("
        "    ID INT AUTOINCREMENT KEY,"
        "    EmpCode INT UNIQUE,"
        "    FirstName VARCHAR DEFAULT \"New\","
        "    LastName VARCHAR,"
        "    Age INT DEFAULT 18,"
        "    Salary DOUBLE DEFAULT 1000.0,"
        "    IsActive BOOL DEFAULT true"
        ");");
    auto result = executor.execute(createStmt);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt1 = Parser::parse(
        "INSERT INTO Employees VALUES (NULL, 101, NULL, \"Doe\", NULL, NULL, "
        "NULL);");
    result = executor.execute(insertStmt1);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
        EXPECT_EQ(std::get<int>(data[0][0]), 0);
        EXPECT_EQ(std::get<int>(data[0][1]), 101);
        EXPECT_EQ(std::get<std::string>(data[0][2]), "New");
        EXPECT_EQ(std::get<std::string>(data[0][3]), "Doe");
        EXPECT_EQ(std::get<int>(data[0][4]), 18);
        EXPECT_EQ(std::get<double>(data[0][5]), 1000.0);
        EXPECT_EQ(std::get<bool>(data[0][6]), true);
    }

    auto insertStmt2 = Parser::parse(
        "INSERT INTO Employees VALUES (NULL, 102, \"John\", \"Smith\", 25, "
        "2000.0, false);");
    result = executor.execute(insertStmt2);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt3 = Parser::parse(
        "INSERT INTO Employees VALUES (NULL, 100 + 3, \"Bob\", \"Johnson\", 20 "
        "+ 5, 1500.0 * 2, true && true);");
    result = executor.execute(insertStmt3);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 3);

        EXPECT_EQ(std::get<int>(data[1][0]), 1);
        EXPECT_EQ(std::get<int>(data[1][1]), 102);
        EXPECT_EQ(std::get<std::string>(data[1][2]), "John");
        EXPECT_EQ(std::get<std::string>(data[1][3]), "Smith");
        EXPECT_EQ(std::get<int>(data[1][4]), 25);
        EXPECT_EQ(std::get<double>(data[1][5]), 2000.0);
        EXPECT_EQ(std::get<bool>(data[1][6]), false);

        EXPECT_EQ(std::get<int>(data[2][0]), 2);
        EXPECT_EQ(std::get<int>(data[2][1]), 103);
        EXPECT_EQ(std::get<std::string>(data[2][2]), "Bob");
        EXPECT_EQ(std::get<std::string>(data[2][3]), "Johnson");
        EXPECT_EQ(std::get<int>(data[2][4]), 25);
        EXPECT_EQ(std::get<double>(data[2][5]), 3000.0);
        EXPECT_EQ(std::get<bool>(data[2][6]), true);
    }

    auto insertStmt4 = Parser::parse(
        "INSERT INTO Employees VALUES (NULL, 101, \"Test\", \"Wilson\", 30, "
        "2500.0, true);");
    result = executor.execute(insertStmt4);
    EXPECT_FALSE(result.is_ok());

    auto insertStmt5 = Parser::parse(
        "INSERT INTO Employees VALUES (100, 104, \"Test\", \"Brown\", 35, "
        "3000.0, true);");
    result = executor.execute(insertStmt5);
    EXPECT_TRUE(result.is_ok());

    {
        const auto& table = db.getTable("Employees");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 4);
        EXPECT_EQ(std::get<int>(data[3][0]), 100);
    }
}

TEST_F(ExecutorTest, ExecuteInsertWithEmptyValues) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Test ("
        "    ID INT,"
        "    Name VARCHAR DEFAULT \"Unknown\","
        "    LastName VARCHAR,"
        "    Age INT DEFAULT 18,"
        "    Score INT"
        ");");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, , \"Doe\", , 95);");
    auto result1 = executor.execute(insertStmt1);
    EXPECT_TRUE(result1.is_ok());

    {
        const auto& table = db.getTable("Test");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
        EXPECT_EQ(std::get<int>(data[0][0]), 1);
        EXPECT_EQ(std::get<std::string>(data[0][1]), "Unknown");
        EXPECT_EQ(std::get<std::string>(data[0][2]), "Doe");
        EXPECT_EQ(std::get<int>(data[0][3]), 18);
        EXPECT_EQ(std::get<int>(data[0][4]), 95);
    }

    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (, \"John\", , 25, );");
    auto result2 = executor.execute(insertStmt2);
    EXPECT_FALSE(result2.is_ok());

    {
        const auto& table = db.getTable("Test");
        const auto data = table.get_rows();
        ASSERT_EQ(data.size(), 1);
    }
}

TEST_F(ExecutorTest, ExecuteBasicSelect) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);

    auto selectStmt = Parser::parse("SELECT * FROM Test;");
    auto result = executor.execute(selectStmt);

    EXPECT_TRUE(result.is_ok());

    auto rows = result.get_payload();
    ASSERT_EQ(rows.size(), 2);
    EXPECT_EQ(std::get<int>(rows[0]["ID"]), 1);
    EXPECT_EQ(std::get<std::string>(rows[0]["Name"]), "Alice");
    EXPECT_EQ(std::get<int>(rows[0]["Age"]), 25);
    EXPECT_EQ(std::get<int>(rows[1]["ID"]), 2);
    EXPECT_EQ(std::get<std::string>(rows[1]["Name"]), "Bob");
    EXPECT_EQ(std::get<int>(rows[1]["Age"]), 30);
}

TEST_F(ExecutorTest, ExecuteSelectWithColumns) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto selectStmt = Parser::parse("SELECT ID, Name FROM Test;");
    auto result = executor.execute(selectStmt);
    EXPECT_TRUE(result.is_ok());

    auto rows = result.get_payload();
    ASSERT_EQ(rows.size(), 1);
    ASSERT_EQ(rows[0].size(), 2);
    EXPECT_EQ(std::get<int>(rows[0]["ID"]), 1);
    EXPECT_EQ(std::get<std::string>(rows[0]["Name"]), "Alice");
}

TEST_F(ExecutorTest, ExecuteSelectWithPredicate) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    auto insertStmt3 =
        Parser::parse("INSERT INTO Test VALUES (3, \"Charlie\", 25);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);
    executor.execute(insertStmt3);

    auto selectStmt = Parser::parse("SELECT * FROM Test WHERE Age == 25;");
    auto result = executor.execute(selectStmt);
    EXPECT_TRUE(result.is_ok());

    auto rows = result.get_payload();
    ASSERT_EQ(rows.size(), 2);
    EXPECT_EQ(std::get<std::string>(rows[0]["Name"]), "Alice");
    EXPECT_EQ(std::get<std::string>(rows[1]["Name"]), "Charlie");
}

TEST_F(ExecutorTest, ExecuteBasicUpdate) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto updateStmt =
        Parser::parse("UPDATE Test SET (Age = 26) WHERE ID == 1;");
    auto result = executor.execute(updateStmt);
    EXPECT_TRUE(result.is_ok());

    auto selectStmt = Parser::parse("SELECT * FROM Test;");
    auto selectResult = executor.execute(selectStmt);
    auto rows = selectResult.get_payload();
    ASSERT_EQ(rows.size(), 1);
    EXPECT_EQ(std::get<int>(rows[0]["Age"]), 26);
}

TEST_F(ExecutorTest, ExecuteUpdateWithMultipleColumns) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto updateStmt = Parser::parse(
        "UPDATE Test SET (Name = \"Alicia\", Age = 26) WHERE ID == 1;");
    auto result = executor.execute(updateStmt);
    EXPECT_TRUE(result.is_ok());

    auto selectStmt = Parser::parse("SELECT * FROM Test;");
    auto selectResult = executor.execute(selectStmt);
    auto rows = selectResult.get_payload();
    ASSERT_EQ(rows.size(), 1);
    EXPECT_EQ(std::get<std::string>(rows[0]["Name"]), "Alicia");
    EXPECT_EQ(std::get<int>(rows[0]["Age"]), 26);
}

TEST_F(ExecutorTest, ExecuteUpdateWithExpression) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto updateStmt =
        Parser::parse("UPDATE Test SET (Age = Age + 1) WHERE ID == 1;");
    auto result = executor.execute(updateStmt);
    EXPECT_TRUE(result.is_ok());

    // auto selectStmt = Parser::parse("SELECT * FROM Test;");
    // auto selectResult = executor.execute(selectStmt);
    // auto rows = selectResult.get_payload();
    // ASSERT_EQ(rows.size(), 1);
    // EXPECT_EQ(std::get<int>(rows[0]["Age"]), 26);
}

TEST_F(ExecutorTest, ExecuteUpdateWithComplexPredicate) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 =
        Parser::parse("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);

    auto updateStmt =
        Parser::parse("UPDATE Test SET (Age = 35) WHERE Age > 25 && ID == 2;");
    auto result = executor.execute(updateStmt);
    EXPECT_TRUE(result.is_ok());

    auto selectStmt = Parser::parse("SELECT * FROM Test WHERE ID == 2;");
    auto selectResult = executor.execute(selectStmt);
    auto rows = selectResult.get_payload();
    ASSERT_EQ(rows.size(), 1);
    EXPECT_EQ(std::get<int>(rows[0]["Age"]), 35);
}

TEST_F(ExecutorTest, ExecuteUpdateWithInvalidColumn) {
    auto createStmt =
        Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt =
        Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    executor.execute(insertStmt);

    auto updateStmt =
        Parser::parse("UPDATE Test SET (InvalidColumn = 30) WHERE ID == 1;");
    auto result = executor.execute(updateStmt);
    EXPECT_FALSE(result.is_ok());
}

TEST_F(ExecutorTest, ExecuteDeleteAllRows) {
    auto createStmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 = Parser::parse("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);

    auto deleteStmt = Parser::parse("DELETE FROM Test;");
    auto result = executor.execute(deleteStmt);
    EXPECT_TRUE(result.is_ok());

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    EXPECT_EQ(data.size(), 0);
}

TEST_F(ExecutorTest, ExecuteDeleteWithPredicate) {
    auto createStmt = Parser::parse("CREATE TABLE Test (ID INT, Name VARCHAR, Age INT);");
    executor.execute(createStmt);

    auto insertStmt1 = Parser::parse("INSERT INTO Test VALUES (1, \"Alice\", 25);");
    auto insertStmt2 = Parser::parse("INSERT INTO Test VALUES (2, \"Bob\", 30);");
    auto insertStmt3 = Parser::parse("INSERT INTO Test VALUES (3, \"Charlie\", 25);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);
    executor.execute(insertStmt3);

    auto deleteStmt = Parser::parse("DELETE FROM Test WHERE Age == 25;");
    auto result = executor.execute(deleteStmt);
    EXPECT_TRUE(result.is_ok());

    const auto& table = db.getTable("Test");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Bob");
}

TEST_F(ExecutorTest, ExecuteDeleteFromNonExistentTable) {
    auto deleteStmt = Parser::parse("DELETE FROM NonExistent WHERE ID == 1;");
    auto result = executor.execute(deleteStmt);
    EXPECT_FALSE(result.is_ok()); 
}

TEST_F(ExecutorTest, ComplexDatabaseOperations) {
    auto createStmt = Parser::parse(
        "CREATE TABLE Employees ("
        "    ID INT AUTOINCREMENT KEY,"
        "    EmpCode INT UNIQUE,"
        "    FirstName VARCHAR DEFAULT \"New\","
        "    LastName VARCHAR,"
        "    Age INT DEFAULT 18,"
        "    Salary DOUBLE DEFAULT 1000.0,"
        "    IsActive BOOL DEFAULT true"
        ");"
    );
    auto result = executor.execute(createStmt);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt1 = Parser::parse("INSERT INTO Employees VALUES (NULL, 101, NULL, \"Doe\", NULL, NULL, NULL);");
    result = executor.execute(insertStmt1);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt2 = Parser::parse("INSERT INTO Employees VALUES (NULL, 102, \"John\", \"Smith\", 25, 2000.0, false);");
    result = executor.execute(insertStmt2);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt3 = Parser::parse("INSERT INTO Employees VALUES (NULL, 100 + 3, \"Bob\", \"Johnson\", 20 + 5, 1500.0 * 2, true && true);");
    result = executor.execute(insertStmt3);
    EXPECT_TRUE(result.is_ok());

    auto insertStmt4 = Parser::parse(
        "INSERT INTO Employees (EmpCode = 104, FirstName = \"Alice\", LastName = \"Brown\", Age = 30, Salary = 3000.0, IsActive = false);"
    );
    result = executor.execute(insertStmt4);
    EXPECT_TRUE(result.is_ok());

    auto updateStmt = Parser::parse("UPDATE Employees SET (Salary = Salary + 500) WHERE Age > 20;");
    result = executor.execute(updateStmt);
    EXPECT_TRUE(result.is_ok());

    auto deleteStmt = Parser::parse("DELETE FROM Employees WHERE Age == 18;");
    result = executor.execute(deleteStmt);
    EXPECT_TRUE(result.is_ok());

    const auto& table = db.getTable("Employees");
    const auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 3);

    auto selectStmt = Parser::parse("SELECT * FROM Employees;");
    auto selectResult = executor.execute(selectStmt);
    auto rows = selectResult.get_payload();
    std::cout << "SELECTED ROWS:" << std::endl;
    for (const auto& row : rows) {
        for (const auto& [key, value] : row) {
            std::cout << key << ": " << dBTypeToString(value) << std::endl;
        }
    }
    std::cout << std::endl;
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<int>(data[0][1]), 102);
    EXPECT_EQ(std::get<std::string>(data[0][2]), "John");
    EXPECT_EQ(std::get<std::string>(data[0][3]), "Smith");
    EXPECT_EQ(std::get<int>(data[0][4]), 25);
    EXPECT_EQ(std::get<double>(data[0][5]), 2500.0);
    EXPECT_EQ(std::get<bool>(data[0][6]), false);

    EXPECT_EQ(std::get<int>(data[1][0]), 2);
    EXPECT_EQ(std::get<int>(data[1][1]), 103);
    EXPECT_EQ(std::get<std::string>(data[1][2]), "Bob");
    EXPECT_EQ(std::get<std::string>(data[1][3]), "Johnson");
    EXPECT_EQ(std::get<int>(data[1][4]), 25);
    EXPECT_EQ(std::get<double>(data[1][5]), 3500.0);
    EXPECT_EQ(std::get<bool>(data[1][6]), true);

    EXPECT_EQ(std::get<int>(data[2][0]), 3);
    EXPECT_EQ(std::get<int>(data[2][1]), 104);
    EXPECT_EQ(std::get<std::string>(data[2][2]), "Alice");
    EXPECT_EQ(std::get<std::string>(data[2][3]), "Brown");
    EXPECT_EQ(std::get<int>(data[2][4]), 30);
    EXPECT_EQ(std::get<double>(data[2][5]), 3500.0);
    EXPECT_EQ(std::get<bool>(data[2][6]), false);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}