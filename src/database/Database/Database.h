//
// Created by QUASARITY on 06.11.2024.
//
#ifndef DATABASE_CONTROLLER_HSE_DATABASE_H
#define DATABASE_CONTROLLER_HSE_DATABASE_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../../types.h"
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

        Table& get_table(const std::string& table_name) {
            return m_tables[table_name];
        }

        void save_to_file(std::ofstream&& stream) {
             struct TableHeader {
                 std::string name;
                 size_t entries_number;
                 std::vector<std::string> keys;
                 std::vector<size_t> sizes;
             };
            for (auto &[name, table]: m_tables) {
                TableHeader header = { name, table.size(), table.get_keys(), table.get_sizes() };
                stream.write(reinterpret_cast<char*>(&header), sizeof(TableHeader));
                for (auto &[key, row]: table.get_rows()) {
                    stream.write(reinterpret_cast<char*>(
                            const_cast<DBType*>(&key)
                    ), sizeof(DBType));
                    for (auto &value: row) {
                        stream.write(reinterpret_cast<char*>(&value), sizeof(DBType));
                    }
                }
            }
        }

        void load_from_file(std::ifstream&& stream) {
            struct TableHeader {
                std::string name;
                size_t entries_number;
                std::vector<std::string> keys;
                std::vector<size_t> sizes;
            };
            while (stream.peek() != EOF) {
                TableHeader header;
                stream.read(reinterpret_cast<char *>(&header), sizeof(TableHeader));
                std::map<DBType, std::vector<DBType>> rows;
                for (size_t i = 0; i < header.entries_number; i++) {
                    DBType key;
                    stream.read(reinterpret_cast<char *>(&key), sizeof(DBType));
                    std::vector<DBType> row;
                    for (size_t j = 0; j < header.keys.size(); j++) {
                        DBType value;
                        stream.read(reinterpret_cast<char *>(&value), sizeof(DBType));
                        row.push_back(value);
                    }
                    rows[key] = row;
                }
                m_tables.insert(
                        std::make_pair(
                                header.name,
                                Table(header.keys, header.sizes, std::move(rows))
                        )
                );
            }
         }
    private:
        std::unordered_map<std::string, Table> m_tables;
    };
} // database

#endif //DATABASE_CONTROLLER_HSE_DATABASE_H
