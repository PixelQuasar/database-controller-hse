//
// Created by QUASARITY on 13.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_RESULT_H
#define DATABASE_CONTROLLER_HSE_RESULT_H

#include <sstream>
#include <iterator>
#include <cstddef>

#include <unordered_map>
#include <vector>

#include "../../types.h"

namespace database {
class Result {
   public:
    Result(std::vector<ResultRowType>&& payload, std::string& error_msg,
           bool is_error)
        : m_is_error(is_error),
          m_error_msg(error_msg),
          m_payload(std::move(payload)) {}

    explicit Result(std::vector<ResultRowType>&& payload)
        : m_payload(std::move(payload)) {}

    Result() : m_is_error(false), m_error_msg({}), m_payload({}) {}

    static Result errorResult(std::string&& msg) { return {{}, msg, true}; }

    bool is_ok() const { return !m_is_error; }

    std::string get_error_message() { return m_error_msg; }

    std::vector<ResultRowType> get_payload() { return m_payload; }

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ResultRowType;
        using pointer = value_type*;  
        using reference  = value_type&;
        Iterator(std::vector<ResultRowType>::iterator ptr) : rptr(ptr) {}
        reference operator*() const { return *rptr; }
        pointer operator->() { return &(*rptr); }

        Iterator& operator++() { rptr++; return *this; }  

        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.rptr == b.rptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.rptr != b.rptr; }; 
        private:
            std::vector<ResultRowType>::iterator rptr;
    };

    Iterator begin() { return Iterator(m_payload.begin()); }
    Iterator end()   { return Iterator(m_payload.end()); } 
private:
    bool m_is_error = false;
    std::string m_error_msg;
    std::vector<ResultRowType> m_payload;
};
} // database


#endif  // DATABASE_CONTROLLER_HSE_RESULT_H
