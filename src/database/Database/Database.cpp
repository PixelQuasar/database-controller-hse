//
// Created by QUASARITY on 06.11.2024.
//

#include "Database.h"

namespace database {

    void Database::createTable(const std::string& name, const SchemeType& columns) {
        if (tables_.find(name) != tables_.end()) {
            throw std::runtime_error("Table already exists: " + name);
        }
        tables_.emplace(name, Table(name, columns));
    }

    void Database::insertInto(const std::string& tableName, const RowType& values) {
        auto it = tables_.find(tableName);
        if (it == tables_.end()) {
            throw std::runtime_error("Table does not exist: " + tableName);
        }
        it->second.insert_row(values);
    }

    const Table& Database::getTable(const std::string& name) const {
        auto it = tables_.find(name);
        if (it == tables_.end()) {
            throw std::runtime_error("Table does not exist: " + name);
        }
        return it->second;
    }

} // namespace database