//
// Created by QUASARITY on 13.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_RESULT_H
#define DATABASE_CONTROLLER_HSE_RESULT_H

#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <cstddef>
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
        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ResultRowType;
            using pointer = std::vector<ResultRowType>*;  
            using reference  = std::vector<ResultRowType>&;
            Iterator(pointer ptr) : rptr(ptr) {}
            reference operator*() const { return *rptr; }
            pointer operator->() { return rptr; }

            Iterator& operator++() { rptr++; return *this; }  

            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.rptr == b.rptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.rptr != b.rptr; }; 
            Iterator begin() { return Iterator(&rptr[0]); }
            Iterator end()   { return Iterator(&rptr[sizeof(m_payload)]); } 
            private:
                pointer rptr;
        };
    private:
        bool m_is_error = false;
        std::string m_error_msg;
        std::vector<ResultRowType> m_payload;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_RESULT_H
