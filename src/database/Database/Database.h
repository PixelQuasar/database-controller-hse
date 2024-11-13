//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <iostream>
#include <vector>
#include "../Schema/Schema.h"
#include "../Result/Result.h"

namespace database {
    class Database {
    public:
        explicit Database() {}

        template<class T>
         Result<T> exec(std::string& query_str) {
            std::vector<std::unique_ptr<Schema>> payload = {};

            return Result<std::vector<std::unique_ptr<Schema>>>(std::move(
                std::make_unique<std::vector<std::unique_ptr<Schema>>>(std::move(payload))
            ));
        }
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_DATABASE_H
