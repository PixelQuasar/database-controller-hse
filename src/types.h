//
// Created by QUASARITY on 17.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TYPES_H
#define DATABASE_CONTROLLER_HSE_TYPES_H

#include <string>
#include <unordered_map>
#include <vector>

namespace database {
    using DBType = std::variant<int, double, bool, std::string>;

    using RowType = std::unordered_map<std::string, DBType>;

//    std::vector<std::vector<std::string>> query_templates = {
//            { "SELECT", "FROM" },
//            { "SELECT", "FROM", "WHERE" },
//            { "INSERT", "INTO", "VALUES" },
//            { "DELETE", "FROM", "WHERE" },
//            { "UPDATE", "SET", "WHERE" },
//            { "JOIN", "ON" },
//            { "CREATE TABLE" }
//    };
} //database

#endif //DATABASE_CONTROLLER_HSE_TYPES_H
