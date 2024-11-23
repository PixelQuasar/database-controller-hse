//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>
#include "../../query_language/AST/SQLStatement.h"

namespace database {

    using DBType = std::variant<int, double, bool, std::string>;

    class Table {
    public:
        Table(const std::string& name, const std::vector<ColumnDefinition>& columns)
            : name_(name), columns_(columns) {}

        void insertRow(const std::vector<DBType>& row) {
            if (row.size() != columns_.size()) {
                throw std::invalid_argument("Row size does not match table columns.");
            }
            data_.emplace_back(row);
        }

        const std::string& getName() const { return name_; }
        const std::vector<ColumnDefinition>& getColumns() const { return columns_; }
        const std::vector<std::vector<DBType>>& getData() const { return data_; }

    private:
        std::string name_;
        std::vector<ColumnDefinition> columns_;
        std::vector<std::vector<DBType>> data_;
    };

    class Database {
    public:
        void createTable(const std::string& name, const std::vector<ColumnDefinition>& columns) {
            if (tables_.find(name) != tables_.end()) {
                throw std::runtime_error("Table already exists: " + name);
            }
            tables_.emplace(name, Table(name, columns));
        }

        void insertInto(const std::string& tableName, const std::vector<DBType>& values) {
            auto it = tables_.find(tableName);
            if (it == tables_.end()) {
                throw std::runtime_error("Table does not exist: " + tableName);
            }
            it->second.insertRow(values);
        }

        const Table& getTable(const std::string& name) const {
            auto it = tables_.find(name);
            if (it == tables_.end()) {
                throw std::runtime_error("Table does not exist: " + name);
            }
            return it->second;
        }

    private:
        std::unordered_map<std::string, Table> tables_;
    };

} // namespace database

#endif // DATABASE_CONTROLLER_HSE_DATABASE_H
