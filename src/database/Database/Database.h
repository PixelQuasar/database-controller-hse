//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <iostream>
#include <vector>
#include "../Schema/Schema.h"

namespace database {

    class Database {
    public:
        explicit Database() {

        }

        template<class T>
        std::vector<std::unique_ptr<T>> exec(std::string& query_str) {
            std::vector<std::unique_ptr<Schema>> result = {};
            return result;
        }
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_DATABASE_H
