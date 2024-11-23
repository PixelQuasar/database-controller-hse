#include "Database.h"
#include <gtest/gtest.h>

using namespace database;

class DatabaseTest : public ::testing::Test {
protected:
    Database db;
};

TEST_F(DatabaseTest, CreateTable) {
    db.createTable("Test", {{"ID", "INT"}, {"Name", "VARCHAR"}});
    const auto& table = db.getTable("Test");
    EXPECT_EQ(table.getName(), "Test");
    ASSERT_EQ(table.getColumns().size(), 2);
    EXPECT_EQ(table.getColumns()[0].name, "ID");
    EXPECT_EQ(table.getColumns()[1].name, "Name");
}

TEST_F(DatabaseTest, InsertIntoTable) {
    db.createTable("Test", {{"ID", "INT"}, {"Name", "VARCHAR"}});
    db.insertInto("Test", {1, "Alice"});
    const auto& table = db.getTable("Test");
    const auto& data = table.getData();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
} 