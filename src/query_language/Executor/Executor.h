//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_EXECUTOR_H
#define DATABASE_CONTROLLER_HSE_EXECUTOR_H

#include "../../database/Database/Database.h"
#include "../AST/SQLStatement.h"
#include <memory>
#include <iostream>
#include "../../Calculator/Calculator.h"

namespace database {

    class Executor {
    public:
        Executor(Database& database) : m_database(database) {}

        void execute(const SQLStatement& stmt);

    private:
        Database& m_database;
    };

} // namespace database

#endif // DATABASE_CONTROLLER_HSE_EXECUTOR_H
