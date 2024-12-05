//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Table/Table.h"

namespace database {

class Database {
   public:
    void createTable(const std::string& name, const SchemeType& columns);
    void insertInto(const std::string& tableName, const RowType& values);
    Table& getTable(const std::string& name);

   private:
    std::unordered_map<std::string, Table> tables_;
};

}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_DATABASE_H