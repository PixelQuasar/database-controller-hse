//
// Created by QUASARITY on 06.11.2024.
//

#include "Executor.h"
#include "../AST/SQLStatement.h"
#include "../../database/Database/Database.h"
#include "../../Calculator/Calculator.h"
#include <iostream>
#include <regex>

namespace database {

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

                if (insertStmt->values.size() != columns.size()) {
                    throw std::runtime_error("Number of values does not match number of columns.");
                }

                for (size_t i = 0; i < insertStmt->values.size(); ++i) {
                    const auto &valStr = insertStmt->values[i];
                    const auto &columnType = columns[i].type;

                    try {
                        auto calc_result = calc.evaluate(valStr);

                        if ((columnType == "INT" && !std::holds_alternative<int>(calc_result)) ||
                            (columnType == "DOUBLE" && !std::holds_alternative<double>(calc_result)) ||
                            (columnType == "BOOL" && !std::holds_alternative<bool>(calc_result)) ||
                            (columnType == "VARCHAR" && !std::holds_alternative<std::string>(calc_result))) {
                            throw std::runtime_error("Type mismatch for column " + columns[i].name);
                        }

                        row.push_back(calc_result);
                    } catch (const std::exception &e) {
                        throw std::runtime_error("Error processing value for column " +
                                                 columns[i].name + ": " + e.what());
                    }
                }
                m_database.insertInto(insertStmt->tableName, row);
            } else if (const auto *insertStmt = dynamic_cast<const SelectStatement *>(&stmt)) {
                auto table = m_database.getTable(insertStmt->tableName);

                if (insertStmt->column_names[0] != "*") {

                }
                auto filter_predicate = [table, insertStmt, calc](const std::vector<DBType>& row) {
                    std::unordered_map<std::string, std::string> row_values = {};
                    for (const auto& [name, index] : table.get_column_to_row_offset()) {
                        std::string str_value;
                        if (std::holds_alternative<int>(row[index])) {
                            str_value = std::to_string(std::get<int>(row[index]));
                        } else if (std::holds_alternative<double>(row[index])) {
                            str_value = std::to_string(std::get<double>(row[index]));
                        } else if (std::holds_alternative<bool>(row[index])) {
                            str_value = std::to_string(std::get<bool>(row[index]));
                        } else if (std::holds_alternative<std::string>(row[index])) {
                            str_value = std::get<std::string>(row[index]);
                        }
                        row_values[name] = str_value;
                    }
                    return calculator::safeGet<bool>(calc.evaluate(insertStmt->predicate, row_values));
                };

                std::vector<ResultRowType> result_rows;

                for (const auto& column : insertStmt->predicate.empty() ? table.filter(filter_predicate) : table.get_rows()) {
                    std::unordered_map<std::string, DBType> row = {};
                    if (insertStmt->column_names[0] == "*") {
                        for (const auto& [name, index] : table.get_column_to_row_offset()) {
                            row[name] = column[index];
                        }
                    } else {
                        for (const auto &column_name: insertStmt->column_names) {
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