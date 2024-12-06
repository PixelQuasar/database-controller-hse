#include <gtest/gtest.h>

#include "Database.h"

using namespace database;

class DatabaseTest : public ::testing::Test {
   protected:
    Database db;
};

TEST_F(DatabaseTest, CreateTable) {
    db.createTable("Test",
                   {{"ID", DataTypeName::INT}, {"Name", DataTypeName::STRING}});
    const auto& table = db.getTable("Test");
    EXPECT_EQ(table.get_name(), "Test");
    ASSERT_EQ(table.get_scheme().size(), 2);
    EXPECT_EQ(table.get_scheme()[0].name, "ID");
    EXPECT_EQ(table.get_scheme()[1].name, "Name");
}

TEST_F(DatabaseTest, InsertIntoTable) {
    db.createTable("Test",
                   {{"ID", DataTypeName::INT}, {"Name", DataTypeName::STRING}});
    db.insertInto("Test", {1, "Alice"});
    auto& table = db.getTable("Test");
    auto& data = table.get_rows();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(std::get<int>(data[0][0]), 1);
    EXPECT_EQ(std::get<std::string>(data[0][1]), "Alice");
}