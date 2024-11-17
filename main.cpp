#include <iostream>
#include <fstream>
#include "src/database/Database/Database.h"

struct User {
    std::string email;
    std::string name;
    int age;
};

using namespace database;

int main() {
    auto db = database::Database();

    db.add_table("users", {"email", "name", "age"}, {sizeof(std::string), sizeof(std::string), sizeof(int)});
    db.get_table("users").insert_many({
        { "user1@example.com", { "user1@example.com", "User One", 25 } },
        { "user2@example.com", { "user2@example.com", "User Two", 30 } },
        { "user3@example.com", { "user3@example.com", "User Three", 22 } }
    });

    db.save_to_file(std::ofstream("data.bin", std::ios::binary));

    std::cout << "hello world!" << std::endl;
    return 0;
}
