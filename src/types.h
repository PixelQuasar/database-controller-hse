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
#include <stdexcept>

namespace database {
    enum DataTypeName {
        INT,
        DOUBLE,
        BOOL,
        STRING,
        BYTEBUFFER
    };

struct ColumnDefinition {
    std::string name;
    DataTypeName type;
    bool notNull = false;
    bool isUnique = false;
    bool isKey = false;
    std::string defaultValue;
    bool isAutoIncrement = false;
    bool hasDefault = false;

    std::string toString() const { return name + " " + dataTypeNameToString(type); }

    static std::string dataTypeNameToString(DataTypeName type) {
        switch (type) {
            case INT:
                return "INT";
            case DOUBLE:
                return "DOUBLE";
            case BOOL:
                return "BOOL";
            case STRING:
                return "VARCHAR";
            case BYTEBUFFER:
                return "BYTEBUFFER";
        }
    }

    static DataTypeName stringToDataTypeName(const std::string& name) {
        if (name == "INT") {
            return INT;
        } else if (name == "DOUBLE") {
            return DOUBLE;
        } else if (name == "BOOL") {
            return BOOL;
        } else if (name == "VARCHAR") {
            return STRING;
        } else if (name == "BYTEBUFFER") {
            return BYTEBUFFER;
        }
        throw std::runtime_error("Unknown type name: " + name);
    }
};

using bytebuffer = std::vector<char>;

using DBType = std::variant<int, double, bool, std::string, bytebuffer>;

using ResultRowType = std::unordered_map<std::string, DBType>;

using RowType = std::vector<DBType>;

using SchemeType = std::vector<ColumnDefinition>;

std::string dBTypeToString(DBType value);
}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_TYPES_H
