//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_SCHEMA_H
#define DATABASE_CONTROLLER_HSE_SCHEMA_H

#include <cstdlib>

namespace database {
    using IdType = size_t;

    class Schema {
    public:
        virtual ~Schema() = default;

        IdType id;
    };

} // database

#endif //DATABASE_CONTROLLER_HSE_SCHEMA_H
