#include "src/database/Database/Database.h"
#include "src/query_language/Executor/Executor.h"

using namespace database;

int main() {
    Database db = Database();
    auto createStmt = ("CREATE TABLE User (ID INT, Name VARCHAR, Age INT);");
    auto createStmt2 =
        ("CREATE TABLE Post (ID INT, AuthorId INT, Text VARCHAR);");
    Executor executor(db);
    executor.execute(createStmt);
    executor.execute(createStmt2);

    auto insertStmt1 = ("INSERT INTO User VALUES (1, \"Alice\", 25);");
    auto insertStmt2 = ("INSERT INTO User VALUES (2, \"Bob\", 30);");
    auto insertStmt3 = ("INSERT INTO User VALUES (3, \"John\", 31);");
    executor.execute(insertStmt1);
    executor.execute(insertStmt2);
    executor.execute(insertStmt3);

    auto insertStmt4 = ("INSERT INTO Post VALUES (1, 1, \"HELLO WORLD 1\");");
    auto insertStmt5 = ("INSERT INTO Post VALUES (2, 1, \"HELLO WORLD 2\");");
    auto insertStmt6 = ("INSERT INTO Post VALUES (3, 3, \"HELLO WORLD 3\");");
    executor.execute(insertStmt4);
    executor.execute(insertStmt5);
    executor.execute(insertStmt6);

    auto selectStmt =
        ("SELECT User.Name, Post.Text FROM User JOIN Post ON User.ID == "
         "Post.AuthorId WHERE User.Name == \"Alice\";");
    auto result = executor.execute(selectStmt);
    std::cout << result.get_error_message() << std::endl;
    for (auto& row : result) {
        for (auto& [key, value] : row) {
            std::cout << key << ":" << dBTypeToString(value) << "   ";
        }
        std::cout << std::endl;
    }
    return 0;
}
