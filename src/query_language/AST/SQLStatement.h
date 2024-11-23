#ifndef DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H
#define DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H

#include <string>
#include <vector>
#include <memory>

namespace database {

    class SQLStatement {
    public:
        virtual ~SQLStatement() = default;
        virtual std::string toString() const = 0;
    };

    struct ColumnDefinition {
        std::string name;
        std::string type;

        std::string toString() const {
            return name + " " + type;
        }
    };

    class CreateTableStatement : public SQLStatement {
    public:
        std::string tableName;
        std::vector<ColumnDefinition> columns;

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

} // namespace database

#endif // DATABASE_CONTROLLER_HSE_SQLSTATEMENT_H 