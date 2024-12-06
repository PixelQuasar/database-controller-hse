//
// Created by QUASARITY on 06.11.2024.
//

#include "Database.h"
#include "../Table/Table.h"

#include <stdexcept>

namespace database {

void Database::createTable(const std::string& name, const SchemeType& columns) {
    if (tables_.find(name) != tables_.end()) {
        throw std::runtime_error("Table already exists: " + name);
    }

    Table table(name, columns);

    for (const auto& column : columns) {
        if (column.isUnique) {
            table.addUniqueConstraint(column.name);
        }
    }

    tables_.emplace(name, std::move(table));
}

void Database::insertInto(const std::string& tableName, const RowType& values) {
    auto it = tables_.find(tableName);
    if (it == tables_.end()) {
        throw std::runtime_error("Table does not exist: " + tableName);
    }
    it->second.insert_row(values);
}

Table& Database::getTable(const std::string& name) {
    auto it = tables_.find(name);
    if (it == tables_.end()) {
        throw std::runtime_error("Table does not exist: " + name);
    }
    return it->second;
}

bool Database::hasTable(const std::string& name) const {
    return tables_.find(name) != tables_.end();
}

void Database::createIndex(const std::string& tableName, const std::string& indexType, const std::vector<std::string>& columns) {
    auto it = tables_.find(tableName);
    if (it == tables_.end()) {
        throw std::runtime_error("Таблица не существует: " + tableName);
    }
    it->second.createIndex(indexType, columns);
}
}  // namespace database