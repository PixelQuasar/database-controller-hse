//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TABLE_H
#define DATABASE_CONTROLLER_HSE_TABLE_H

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <streambuf>
#include "../types.h"

namespace database {
    class Table {
    public:
        Table() {}

        Table(
                const std::vector<std::string>& keys,
                const std::vector<size_t>& sizes,
                const std::map<DBType, std::vector<DBType>>&& rows
            ) : m_row_sizes(sizes), m_rows(std::move(rows)) {
            for (int i = 0; i < keys.size(); i++) {
                m_key_indexes[keys[i]] = i;
            }
        }

        Table(
                const std::vector<std::string>& keys,
                const std::vector<size_t> sizes
            ) : m_row_sizes(sizes) {
            for (int i = 0; i < keys.size(); i++) {
                m_key_indexes[keys[i]] = i;
            }
        }

        void insert(const DBType& key, const std::vector<DBType>& row) {
            m_rows.insert(std::make_pair(key, row));
        }

        void remove(const DBType& key) {
            m_rows.erase(key);
        }

        void update(const DBType& key, const std::vector<DBType>& row) {
            m_rows[key] = row;
        }

        std::vector<DBType> get(const DBType& key) {
            return m_rows[key];
        }

        std::string convert_to_byte_buffer();

        void load_from_byte_buffer(const std::string& buffer);
    private:
        std::unordered_map<std::string, int> m_key_indexes;
        const int m_key_index = 0;
        std::map<DBType, std::vector<DBType>> m_rows;
        std::vector<size_t> m_row_sizes;
    };

} // database

#endif //DATABASE_CONTROLLER_HSE_TABLE_H
