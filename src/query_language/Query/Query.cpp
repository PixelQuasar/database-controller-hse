//
// Created by QUASARITY on 12.11.2024.
//

#include "Query.h"

namespace database {
const std::map<std::string, QueryType> Query::query_templates = {
    {"SELECT FROM ", QueryType::SELECT},
    {"SELECT FROM WHERE ", QueryType::SELECT_WHERE},
    {"INSERT INTO VALUES ", QueryType::INSERT},
    {"DELETE FROM WHERE ", QueryType::DELETE_WHERE},
    {"UPDATE SET WHERE ", QueryType::UPDATE_SET_WHERE},
    {"JOIN ON ", QueryType::JOIN_ON},
    {"CREATE TABLE ", QueryType::CREATE_TABLE}};

const std::vector<std::string> Query::possible_key_words = {
    "SELECT", "FROM", "WHERE", "INSERT", "INTO",   "VALUES", "DELETE",
    "UPDATE", "SET",  "JOIN",  "ON",     "CREATE", "TABLE"};
}  // namespace database