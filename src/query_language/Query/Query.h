//
// Created by QUASARITY on 12.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_QUERY_H
#define DATABASE_CONTROLLER_HSE_QUERY_H
#include <string>
#include <vector>
#include <map>

namespace database {
    enum QueryType {
        SELECT,
        INSERT,
        DELETE,
        UPDATE,
        JOIN,
        CREATE,
    };

    class Query {
    public:
        explicit Query(std::string) {

        }

    private:
        QueryType m_query_type;
        std::vector<std::string> raw_params;
    };
}

#endif //DATABASE_CONTROLLER_HSE_QUERY_H
