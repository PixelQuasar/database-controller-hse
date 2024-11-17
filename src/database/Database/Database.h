//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "../types.h"
#include "../Result/Result.h"
#include "../Table/Table.h"

namespace database {
    class Database {
    public:
         Database() = default;

         Result exec(std::string& query_str) {
             std::vector<RowType> users = {
                     { {"email", "user1@example.com"}, {"name", "User One"}, {"age", 25} },
                     { {"email", "user2@example.com"}, {"name", "User Two"}, {"age", 30} },
                     { {"email", "user3@example.com"}, {"name", "User Three"}, {"age", 22} }
             };

            return Result({});
        }

        void add_table(const std::string& table_name, const std::vector<std::string>& keys, const std::vector<size_t>& sizes) {
             m_tables.insert(std::make_pair(table_name, Table(keys, sizes)));
        }

        void delete_table(const std::string& table_name) {
            m_tables.erase(table_name);
        }

        Table& get_table(const std::string& table_name) {
            return m_tables[table_name];
        }
    private:
        std::unordered_map<std::string, Table> m_tables;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_DATABASE_H
