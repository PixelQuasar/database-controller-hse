#ifndef DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H
#define DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../types.h"

namespace database {
class ColumnStatement {
   public:
    std::string name;
    std::string table = "";

    bool operator==(const ColumnStatement& other) const {
        return (name == other.name && table == other.table);
    }
};
}  // namespace database

template <>
struct std::hash<database::ColumnStatement> {
    std::size_t operator()(const database::ColumnStatement& k) const {
        using std::hash;
        using std::size_t;
        using std::string;

        return (hash<string>()(k.name) ^ (hash<string>()(k.table) << 1)) >> 1;
    }
};

namespace database {
class SQLStatement {
   public:
    virtual ~SQLStatement() = default;
    virtual std::string toString() const = 0;
};

class CreateTableStatement : public SQLStatement {
   public:
    std::string tableName;
    SchemeType columns;

    std::string toString() const override {
        std::string result = "CREATE TABLE " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            result += columns[i].toString();
            if (i != columns.size() - 1) {
                result += ", ";
            }
        }
        result += ");";
        return result;
    }
};

class InsertStatement : public SQLStatement {
   public:
    std::string tableName;
    std::vector<std::string> values;
    std::unordered_map<std::string, std::string> columnValuePairs;
    bool isMapFormat = false;

    std::string toString() const override {
        std::string result = "INSERT INTO " + tableName + " VALUES (";
        for (size_t i = 0; i < values.size(); ++i) {
            result += values[i];
            if (i != values.size() - 1) {
                result += ", ";
            }
        }
        result += ");";
        return result;
    }
};

class SelectStatement : public SQLStatement {
   public:
    std::string tableName;
    std::vector<ColumnStatement> columnData;

    // WHERE properties
    std::string predicate;

    // JOIN properties
    std::string foreignTableName;
    std::string joinPredicate;

    std::string toString() const override {
        std::string result = "SELECT ";
        for (size_t i = 0; i < columnData.size(); ++i) {
            if (!columnData[i].table.empty()) {
                result += columnData[i].table + ".";
            }
            result += columnData[i].name;
            if (i != columnData.size() - 1) {
                result += ", ";
            }
        }
        result += " FROM " + tableName;
        if (!predicate.empty()) {
            result += " WHERE " + predicate;
        }
        result += ";";
        return result;
    }
};

class UpdateStatement : public SQLStatement {
   public:
    std::string tableName;
    std::unordered_map<ColumnStatement, std::string> newValues;

    // WHERE properties
    std::string predicate;

    // JOIN properties
    std::string foreignTableName;
    std::string joinPredicate;

    std::string toString() const override {
        std::string result = "UPDATE ";
        bool isFirst = true;
        for (const auto& [column, value] : newValues) {
            if (!isFirst) {
                result += ", ";
            }
            if (!column.table.empty()) {
                result += column.table + ".";
            }
            result += column.name + " = " + value;
            isFirst = false;
        }
        result += " FROM " + tableName;
        if (!predicate.empty()) {
            result += " WHERE " + predicate;
        }
        result += ";";
        return result;
    }
};

class DeleteStatement : public SQLStatement {
   public:
    std::string tableName;
    std::string predicate;

    std::string toString() const override {
        std::string result = "DELETE FROM " + tableName;
        if (!predicate.empty()) {
            result += " WHERE " + predicate;
        }
        result += ";";
        return result;
    }
};

}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H