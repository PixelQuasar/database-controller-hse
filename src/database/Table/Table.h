//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TABLE_H
#define DATABASE_CONTROLLER_HSE_TABLE_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "../../types.h"

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

        size_t size() {
            return m_rows.size();
        }

        std::vector<std::string> get_keys() {
            std::vector<std::string> keys;
            for (auto& [key, _]: m_key_indexes) {
                keys.push_back(key);
            }
            return keys;
        }

        std::vector<size_t> get_sizes() {
            return m_row_sizes;
        }

        std::map<DBType, std::vector<DBType>> get_rows() {
            return m_rows;
        }

        void insert(const DBType& key, const std::vector<DBType>& row) {
            m_rows.insert(std::make_pair(key, row));
        }

        void insert_many(const std::vector<std::pair<DBType, std::vector<DBType>>>& rows) {
            for (auto& row : rows) {
                m_rows.insert(row);
            }
        }

        std::vector<DBType> get_by_key(const DBType& key) {
            return m_rows[key];
        }

        std::vector<std::vector<DBType>> filter(
                const std::function<bool(const std::vector<DBType>&)>& predicate
        );

        void update_many(
                const std::function<void(std::vector<DBType>&)>& updater,
                const std::function<bool(const std::vector<DBType>&)>& predicate
        );

        void remove_many(
            const std::function<bool(const std::vector<DBType>&)>& predicate
        );

        std::string convert_to_byte_buffer();

        void load_from_byte_buffer(const std::string& buffer);
    private:
        std::map<std::string, int> m_key_indexes;
        const int m_key_index = 0;
        std::map<DBType, std::vector<DBType>> m_rows;
        std::vector<size_t> m_row_sizes;
    };

} // database

#endif //DATABASE_CONTROLLER_HSE_TABLE_H
