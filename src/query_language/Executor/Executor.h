//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_QUERYBUILDER_H
#define DATABASE_CONTROLLER_HSE_QUERYBUILDER_H

#include <string>
#include <vector>
#include "../../database/Database/Database.h"
#include "../Query/Query.h"

namespace database {
    class Context {

    };

    class Executor {
    public:
        explicit Executor (Query& query, Database& database) : m_query(&query), m_database(&database) {

        }
    private:
        std::shared_ptr<Query> m_query;
        std::shared_ptr<Database> m_database;
        Context m_query_context = {};
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_QUERYBUILDER_H
