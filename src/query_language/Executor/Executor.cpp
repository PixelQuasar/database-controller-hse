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
            if (const auto *createStmt = dynamic_cast<const CreateTableStatement *>(&stmt)) {
                m_database.createTable(createStmt->tableName, createStmt->columns);
            } else if (const auto *insertStmt = dynamic_cast<const InsertStatement *>(&stmt)) {
                std::vector<DBType> row;
                calculator::Calculator calc;
                const auto &table = m_database.getTable(insertStmt->tableName);
                const auto &columns = table.get_scheme();

                if (!insertStmt->columnValuePairs.empty()) {
                    // Вставка с использованием columnValuePairs
                    row.resize(columns.size());
                    for (const auto& [columnName, valueStr] : insertStmt->columnValuePairs) {
                        auto it = std::find_if(columns.begin(), columns.end(), [&](const ColumnDefinition& col) {
                            return col.name == columnName;
                        });
                        if (it == columns.end()) {
                            throw std::runtime_error("Column not found: " + columnName);
                        }
                        size_t index = std::distance(columns.begin(), it);
                        DBType value;

                        if (valueStr == "NULL") {
                            value = DBType();
                        } else {
                            value = calc.evaluate(valueStr);
                        }

                        // Проверяем типы
                        if ((it->type == "INT" && !std::holds_alternative<int>(value)) ||
                            (it->type == "DOUBLE" && !std::holds_alternative<double>(value)) ||
                            (it->type == "BOOL" && !std::holds_alternative<bool>(value)) ||
                            (it->type == "VARCHAR" && !std::holds_alternative<std::string>(value))) {
                            throw std::runtime_error("Type mismatch for column " + columnName);
                        }

                        row[index] = value;
                    }
                } else {
                    // Существующая логика для VALUES
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
                }
                m_database.insertInto(insertStmt->tableName, row);
            } else {
                throw std::runtime_error("Unsupported SQL statement.");
            }
        } catch (const std::exception &e) {
            result = Result::errorResult(std::string(e.what()));
        }
        return result;
    }

} // namespace database