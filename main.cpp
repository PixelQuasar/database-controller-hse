#include <iostream>
#include "src/database/Database/Database.h"
#include "src/database/Schema/Schema.h"
#include "src/database/Result/Result.h"

class User : public database::Schema {
public:
    std::string email;
    std::string name;
    int age;
};

using namespace database;

int main() {
    auto db = new database::Database();

    std::string query = " SELECT * FROM users";

    auto response = db->exec<std::vector<std::unique_ptr<database::Schema>>>(query);

    std::cout << "hello world!" << std::endl;
    return 0;
}
