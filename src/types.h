//
// Created by QUASARITY on 17.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TYPES_H
#define DATABASE_CONTROLLER_HSE_TYPES_H

#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace database {
struct ColumnDefinition {
    std::string name;
    std::string type;
    bool notNull = false;
    bool isUnique = false;
    bool isKey = false;
    std::string defaultValue;
    bool isAutoIncrement = false;
    bool hasDefault = false;

    std::string toString() const { return name + " " + type; }
};

using DBType = std::variant<int, double, bool, std::string>;

using ResultRowType = std::unordered_map<std::string, DBType>;

using RowType = std::vector<DBType>;

using SchemeType = std::vector<ColumnDefinition>;

std::string dBTypeToString(DBType value);
}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_TYPES_H
