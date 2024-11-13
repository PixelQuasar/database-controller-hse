#include <iostream>
#include "src/database/Database/Database.h"
#include "src/database/Schema/Schema.h"

class User : public database::Schema {
public:
    std::string email;
    std::string name;
    int age;
};

int main() {
    auto db = new database::Database();

    std::string query = " SELECT name FROM users";

    auto names = db->exec<database::Schema>(query);

    std::cout << "hello world!" << std::endl;
    return 0;
}
