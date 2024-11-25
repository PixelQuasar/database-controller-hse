//
// Created by QUASARITY on 06.11.2024.
//

#include "Executor.h"
#include "../AST/SQLStatement.h"
#include "../../database/Database/Database.h"
#include "../../Calculator/Calculator.h"
#include "../Result/Result.h"
#include <iostream>
#include <regex>

namespace database {
    std::string dBTypeToString(DBType value) {
        std::string str_value;
        if (std::holds_alternative<int>(value)) {
            str_value = std::to_string(std::get<int>(value));
         } else if (std::holds_alternative<double>(value)) {
            str_value = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            str_value = std::to_string(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            str_value = std::get<std::string>(value);
        }
        return str_value;
    }

    Result Executor::execute(const SQLStatement& stmt) {
        Result result = {};
        try {
            calculator::Calculator calc;
            if (const auto *createStmt = dynamic_cast<const CreateTableStatement *>(&stmt)) {
                m_database.createTable(createStmt->tableName, createStmt->columns);
            } else if (const auto *insertStmt = dynamic_cast<const InsertStatement *>(&stmt)) {
                std::vector<DBType> row;
                const auto &table = m_database.getTable(insertStmt->tableName);
                const auto &columns = table.get_scheme();

                if (insertStmt->values.size() > columns.size()) {
                    throw std::runtime_error("Too many values provided.");
                }

                for (size_t i = 0; i < columns.size(); ++i) {
                    const auto &columnType = columns[i].type;

                    try {
                        DBType value;
                        
                        if (i < insertStmt->values.size()) {
                            if (insertStmt->values[i] == "NULL" && columns[i].isAutoIncrement) {
                                value = 0;
                            } else {
                                value = calc.evaluate(insertStmt->values[i]);
                            }
                        } else if (columns[i].isAutoIncrement) {
                            value = 0;
                        } else {
                            throw std::runtime_error("No value provided for column " + columns[i].name);
                        }

                        if ((columnType == "INT" && !std::holds_alternative<int>(value)) ||
                            (columnType == "DOUBLE" && !std::holds_alternative<double>(value)) ||
                            (columnType == "BOOL" && !std::holds_alternative<bool>(value)) ||
                            (columnType == "VARCHAR" && !std::holds_alternative<std::string>(value))) {
                            throw std::runtime_error("Type mismatch for column " + columns[i].name);
                        }

                        row.push_back(value);
                    } catch (const std::exception &e) {
                        throw std::runtime_error("Error processing value for column " +
                                               columns[i].name + ": " + e.what());
                    }
                }
                m_database.insertInto(insertStmt->tableName, row);
            } else if (const auto *insertStmt = dynamic_cast<const SelectStatement *>(&stmt)) {
                auto table = m_database.getTable(insertStmt->tableName);

                // check if columns are valid
                 for (const auto &column_name: insertStmt->columnNames) {
                    if (!table.get_column_to_row_offset().count(column_name)) {
                        throw std::invalid_argument("Invalid selector: " + column_name + ".");
                    }
                 }

                // build the predicate
                auto filter_predicate = [table, insertStmt, calc](const std::vector<DBType>& row) {
                    std::unordered_map<std::string, std::string> row_values = {};
                    for (const auto& [name, index] : table.get_column_to_row_offset()) {
                        row_values[name] = dBTypeToString(row[index]);
                    }
                    return calculator::safeGet<bool>(calc.evaluate(insertStmt->predicate, row_values));
                };

                std::vector<ResultRowType> result_rows;

                for (const auto& column : !insertStmt->predicate.empty() ? table.filter(filter_predicate) : table.get_rows()) {
                    std::unordered_map<std::string, DBType> row = {};
                    if (insertStmt->columnNames[0] == "*") {
                        for (const auto& [name, index] : table.get_column_to_row_offset()) {
                            row[name] = column[index];
                        }
                    } else {
                        for (const auto &column_name: insertStmt->columnNames) {
                            row[column_name] = column[table.get_column_to_row_offset()[column_name]];
                        }
                    }
                    result_rows.emplace_back(row);
                }

                result = Result(std::move(result_rows));
            } else {
                throw std::runtime_error("Unsupported SQL statement.");
            }
        } catch (const std::exception &e) {
            result = Result::errorResult(std::string(e.what()));
        }
        return result;
    }

} // namespace database