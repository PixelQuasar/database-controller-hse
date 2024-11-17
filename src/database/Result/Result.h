//
// Created by QUASARITY on 13.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_RESULT_H
#define DATABASE_CONTROLLER_HSE_RESULT_H

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include "../../types.h"

namespace database {
    class Result {
    public:
        Result(std::vector<RowType>&& payload, std::string& error_msg) : m_payload(std::move(payload)), m_error_msg(error_msg) {}

        explicit Result(std::vector<RowType>&& payload) : m_payload(std::move(payload)) {}

        static Result errorResult (std::string& msg) {
            return { {}, msg };
        }
    private:
        std::string m_error_msg;
        std::vector<database::RowType> m_payload;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_RESULT_H
