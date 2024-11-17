//
// Created by QUASARITY on 06.11.2024.
//

#include "Table.h"

namespace database {
    std::string Table::convert_to_byte_buffer() {
        std::string convert_to_byte_buffer() {
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
                    } converter;

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
                    } converter;
                    std::memcpy(converter.charArray4, buffer.data() + offset, 4);
                    row.push_back(converter.intValue);
                    offset += 4;
                } else if (size == 8) {
                    union {
                        double doubleValue;
                        char charArray8[8];
                    } converter;
                    std::memcpy(converter.charArray8, buffer.data() + offset, 8);
                    row.push_back(converter.doubleValue);
                    offset += 8;
                } else {
                    std::string str(buffer.data() + offset, size);
                    row.push_back(str);
                    offset += size;
                }
            }
            rows[row[0]] = row;
        }
    }
} // database