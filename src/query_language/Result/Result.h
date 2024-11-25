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
        Result(std::vector<ResultRowType>&& payload, std::string& error_msg, bool is_error)
            : m_payload(std::move(payload))
            , m_error_msg(error_msg)
            , m_is_error(is_error) {}

        explicit Result(std::vector<ResultRowType>&& payload)
            : m_payload(std::move(payload)) {}

        Result()
            : m_payload({})
            , m_error_msg({})
            , m_is_error(false) {}

        static Result errorResult (std::string&& msg) { return { {}, msg, true }; }

        bool is_ok() const { return !m_is_error; }

        std::string get_error_message() { return m_error_msg; }

        std::vector<ResultRowType> get_payload() { return m_payload; }
    private:
        bool m_is_error = false;
        std::string m_error_msg;
        std::vector<ResultRowType> m_payload;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_RESULT_H
