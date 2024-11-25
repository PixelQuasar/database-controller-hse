//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_EXECUTOR_H
#define DATABASE_CONTROLLER_HSE_EXECUTOR_H

#include <iostream>
#include <memory>
#include <unordered_set>

#include "../../Calculator/Calculator.h"
#include "../../database/Database/Database.h"
#include "../AST/SQLStatement.h"
#include "../Result/Result.h"

namespace database {
class Executor {
   public:
    Executor(Database& database) : m_database(database) {}

    Result execute(std::shared_ptr<SQLStatement> stmt);

   private:
    Database& m_database;
};

}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_EXECUTOR_H
