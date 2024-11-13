//
// Created by QUASARITY on 13.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_RESULT_H
#define DATABASE_CONTROLLER_HSE_RESULT_H

#include <string>

namespace database {
    template <typename T>
    class Result {
    public:
        explicit Result(std::unique_ptr<T> payload) : m_payload(std::move(payload)), m_error_msg({}) {}

        explicit Result(std::unique_ptr<T> payload, std::string& error_msg) : m_payload(payload), m_error_msg(error_msg) {}

        static Result ErrorResult(std::string& error_msg) {
            return Result({}, error_msg);
        }
    private:
        std::string m_error_msg;
        std::unique_ptr<T> m_payload;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_RESULT_H
