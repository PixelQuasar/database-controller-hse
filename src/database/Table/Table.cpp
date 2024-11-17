//
// Created by QUASARITY on 06.11.2024.
//

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "Table.h"

namespace database {
    std::string Table::convert_to_byte_buffer() {
        std::string buffer;
        for (auto& row : m_rows) {
            int current_column_index = 0;
            for (auto& cell : row.second) {
                union {
                    int intValue;
                    double doubleValue;
                    bool boolValue;
                    char charArray4[4];
                    char charArray8[8];
                } converter {};

                if (std::holds_alternative<int>(cell)) {
                    converter.intValue = std::get<int>(cell);
                    buffer.append(converter.charArray4, 4);
                }
                if (std::holds_alternative<double>(cell)) {
                    converter.doubleValue = std::get<double>(cell);
                    buffer.append(converter.charArray8, 8);
                }
                if (std::holds_alternative<bool>(cell)) {
                    converter.boolValue = std::get<bool>(cell);
                    buffer.append(converter.charArray4, 4);
                }
                if (std::holds_alternative<std::string>(cell)) {
                    std::string str = std::get<std::string>(cell);
                    str.resize(m_row_sizes[current_column_index]);
                    buffer.append(str);
                }
                // TODO add buffer type;

                ++current_column_index;
            }
        }
        return buffer;
    }

    void Table::load_from_byte_buffer(const std::string &buffer) {
        std::map<DBType, std::vector<DBType>> rows;
        size_t offset = 0;
        while (offset < buffer.size()) {
            std::vector<DBType> row;
            for (size_t size: m_row_sizes) {
                if (size == 4) {
                    union {
                        int intValue;
                        char charArray4[4];
                    } converter {};
                    std::memcpy(converter.charArray4, buffer.data() + offset, 4);
                    row.emplace_back(converter.intValue);
                    offset += 4;
                } else if (size == 8) {
                    union {
                        double doubleValue;
                        char charArray8[8];
                    } converter {};
                    std::memcpy(converter.charArray8, buffer.data() + offset, 8);
                    row.emplace_back(converter.doubleValue);
                    offset += 8;
                } else {
                    std::string str(buffer.data() + offset, size);
                    row.emplace_back(str);
                    offset += size;
                }
            }
            rows[row[0]] = row;
        }
    }

    std::vector<std::vector<DBType>> Table::filter(
            const std::function<bool(const std::vector<DBType>&)>& predicate
    ) {
        std::vector<std::vector<DBType>> result;
        for (auto& row : m_rows) {
            if (predicate(row.second)) {
                result.push_back(row.second);
            }
        }
        return result;
    }

    void Table::update_many(
            const std::function<void(std::vector<DBType>&)>& updater,
            const std::function<bool(const std::vector<DBType>&)>& predicate
    ) {
        for (auto& row : m_rows) {
            if (predicate(row.second)) {
                updater(row.second);
            }
        }
    }

    void Table::remove_many(
            const std::function<bool(const std::vector<DBType>&)>& predicate
    ) {
        std::vector<DBType> keys_to_remove;
        for (auto& row : m_rows) {
            if (predicate(row.second)) {
                keys_to_remove.push_back(row.first);
            }
        }
        for (auto& key : keys_to_remove) {
            m_rows.erase(key);
        }
    }

} // database
