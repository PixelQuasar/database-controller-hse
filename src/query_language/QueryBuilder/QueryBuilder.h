//
// Created by QUASARITY on 06.11.2024.
//
#include <string>
#include <vector>

#ifndef DATABASE_CONTROLLER_HSE_QUERYBUILDER_H
#define DATABASE_CONTROLLER_HSE_QUERYBUILDER_H

namespace database {
    class Context {

    };

    class QueryBuilder {
    public:
        explicit QueryBuilder (std::vector<std::string>& tokens) : m_tokens(tokens) {

        }

        static QueryBuilder BuildFromString(std::string& string_query) {
            std::vector<std::string> new_tokens = {};

            return QueryBuilder(new_tokens);
        }
    private:
        std::vector<std::string> m_tokens = {};
        Context m_query_context = {};
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_QUERYBUILDER_H
