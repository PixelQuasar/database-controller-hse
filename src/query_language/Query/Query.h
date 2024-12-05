//
// Created by QUASARITY on 12.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_QUERY_H
#define DATABASE_CONTROLLER_HSE_QUERY_H

#include <map>
#include <string>
#include <vector>

namespace database {
enum QueryType {
    SELECT,
    SELECT_WHERE,
    INSERT,
    DELETE_WHERE,
    UPDATE_SET_WHERE,
    JOIN_ON,
    CREATE_TABLE
};

class Query {

};
}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_QUERY_H
