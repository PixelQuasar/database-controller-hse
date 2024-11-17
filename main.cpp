#include <iostream>
#include "src/database/Database/Database.h"
#include "src/database/Result/Result.h"

struct User {
    std::string email;
    std::string name;
    int age;
};

using namespace database;

int main() {
    auto db = database::Database();

    std::string query = " SELECT * FROM users";

    auto response = db.exec(query);

    std::cout << "hello world!" << std::endl;
    return 0;
}
