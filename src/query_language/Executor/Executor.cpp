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

    Result Executor::execute(const SQLStatement& stmt) {
        Result result = {};
        try {
            calculator::Calculator calc;
            if (const auto *createStmt = dynamic_cast<const CreateTableStatement *>(&stmt)) {
                std::cout << "Executing CREATE TABLE statement" << std::endl;
                m_database.createTable(createStmt->tableName, createStmt->columns);
            } else if (const auto *insertStmt = dynamic_cast<const InsertStatement *>(&stmt)) {
                std::cout << "Executing INSERT statement" << std::endl;
                const auto &table = m_database.getTable(insertStmt->tableName);
                const auto &columns = table.get_scheme();
                auto offsets = table.get_column_to_row_offset();

                if (!insertStmt->isMapFormat) {
                    std::cout << "Processing VALUES format" << std::endl;
                    if (insertStmt->values.size() > columns.size()) {
                        throw std::runtime_error("Too many values provided");
                    }

                    std::vector<DBType> row(columns.size());
                    for (size_t i = 0; i < columns.size(); ++i) {
                        const auto &column = columns[i];
                        try {
                            if (i < insertStmt->values.size()) {
                                if (insertStmt->values[i] == "NULL") {
                                    if (column.isAutoIncrement) {
                                        row[i] = 0;
                                    } else if (column.hasDefault) {
                                        row[i] = calc.evaluate(column.defaultValue);
                                    } else {
                                        if (column.type == "INT") row[i] = 0;
                                        else if (column.type == "DOUBLE") row[i] = 0.0;
                                        else if (column.type == "BOOL") row[i] = false;
                                        else if (column.type == "VARCHAR") row[i] = std::string("");
                                    }
                                } else {
                                    auto value = calc.evaluate(insertStmt->values[i]);
                                    if ((column.type == "INT" && !std::holds_alternative<int>(value)) ||
                                        (column.type == "DOUBLE" && !std::holds_alternative<double>(value)) ||
                                        (column.type == "BOOL" && !std::holds_alternative<bool>(value)) ||
                                        (column.type == "VARCHAR" && !std::holds_alternative<std::string>(value))) {
                                        throw std::runtime_error("Type mismatch for column " + column.name);
                                    }
                                    row[i] = value;
                                }
                            } else if (column.isAutoIncrement) {
                                row[i] = 0;
                            } else if (column.hasDefault) {
                                row[i] = calc.evaluate(column.defaultValue);
                            } else {
                                throw std::runtime_error("No value provided for column " + column.name);
                            }
                        } catch (const std::exception &e) {
                            throw std::runtime_error("Error processing value for column " + column.name + ": " + e.what());
                        }
                    }
                    m_database.insertInto(insertStmt->tableName, row);
                } else {
                    std::cout << "Processing column assignments format" << std::endl;
                    std::vector<DBType> row(columns.size());
                    std::unordered_set<std::string> providedColumns;

                    // Проверяем дубликаты колонок перед обработкой значений
                    for (const auto& [columnName, value] : insertStmt->columnValuePairs) {
                        std::cout << "Checking column: " << columnName << std::endl;
                        if (!offsets.count(columnName)) {
                            std::cout << "Error: Unknown column: " << columnName << std::endl;
                            throw std::runtime_error("Unknown column: " + columnName);
                        }
                        if (!providedColumns.insert(columnName).second) {
                            std::cout << "Error: Duplicate column in assignment: " << columnName << std::endl;
                            throw std::runtime_error("Duplicate column in assignment: " + columnName);
                        }
                    }

                    // Проверяем, что все необходимые колонки предоставлены
                    for (const auto& column : columns) {
                        if (!column.hasDefault && !column.isAutoIncrement && !providedColumns.count(column.name)) {
                            std::cout << "Error: Missing required column: " << column.name << std::endl;
                            throw std::runtime_error("Missing value for required column: " + column.name);
                        }
                    }

                    // Инициализируем значения по умолчанию и автоинкремент
                    for (size_t i = 0; i < columns.size(); ++i) {
                        const auto& column = columns[i];
                        if (!providedColumns.count(column.name)) {  // Если значение не предоставлено
                            if (column.isAutoIncrement) {
                                row[i] = 0;  // Будет заменено на правильное значение в Table::insert_row
                            }
                            else if (column.hasDefault) {
                                try {
                                    row[i] = calc.evaluate(column.defaultValue);
                                } catch (const std::exception& e) {
                                    throw std::runtime_error("Error evaluating default value for column " + 
                                        column.name + ": " + e.what());
                                }
                            } else {
                                if (column.type == "INT") row[i] = 0;
                                else if (column.type == "DOUBLE") row[i] = 0.0;
                                else if (column.type == "BOOL") row[i] = false;
                                else if (column.type == "VARCHAR") row[i] = std::string("");
                            }
                        }
                    }

                    // Вычисляем значения для предоставленных колонок
                    for (const auto& [columnName, valueExpr] : insertStmt->columnValuePairs) {
                        size_t columnIndex = offsets[columnName];
                        const auto& column = columns[columnIndex];
                        
                        try {
                            std::cout << "Evaluating " << columnName << " = " << valueExpr << std::endl;
                            auto value = calc.evaluate(valueExpr);
                            std::cout << "Evaluated value type check for column " << columnName << std::endl;
                            
                            if ((column.type == "INT" && !std::holds_alternative<int>(value)) ||
                                (column.type == "DOUBLE" && !std::holds_alternative<double>(value)) ||
                                (column.type == "BOOL" && !std::holds_alternative<bool>(value)) ||
                                (column.type == "VARCHAR" && !std::holds_alternative<std::string>(value))) {
                                std::cout << "Error: Type mismatch for column " << columnName << std::endl;
                                throw std::runtime_error("Type mismatch for column " + columnName);
                            }
                            row[columnIndex] = value;
                        } catch (const std::exception& e) {
                            std::cout << "Error evaluating " << columnName << ": " << e.what() << std::endl;
                            throw std::runtime_error("Error processing value for column " + columnName + ": " + e.what());
                        }
                    }

                    std::cout << "Inserting row into table" << std::endl;
                    m_database.insertInto(insertStmt->tableName, row);
                }
            } else if (const auto *selectStmt = dynamic_cast<const SelectStatement *>(&stmt)) {
                auto table = m_database.getTable(selectStmt->tableName);

                // check if columns are valid
                 for (const auto &column_name: selectStmt->columnNames) {
                    if (!table.get_column_to_row_offset().count(column_name)) {
                        throw std::invalid_argument("Invalid selector: " + column_name + ".");
                    }
                 }

                // build the predicate
                auto filter_predicate = [table, selectStmt, calc](const std::vector<DBType>& row) {
                    std::unordered_map<std::string, std::string> row_values = {};
                    for (const auto& [name, index] : table.get_column_to_row_offset()) {
                        row_values[name] = dBTypeToString(row[index]);
                    }
                    return calculator::safeGet<bool>(calc.evaluate(selectStmt->predicate, row_values));
                };

                std::vector<ResultRowType> result_rows;

                for (const auto& column : !selectStmt->predicate.empty() ? table.filter(filter_predicate) : table.get_rows()) {
                    std::unordered_map<std::string, DBType> row = {};
                    if (selectStmt->columnNames[0] == "*") {
                        for (const auto& [name, index] : table.get_column_to_row_offset()) {
                            row[name] = column[index];
                        }
                    } else {
                        for (const auto &column_name: selectStmt->columnNames) {
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
            std::cout << "Error in execute: " << e.what() << std::endl;
            result = Result::errorResult(std::string(e.what()));
        }
        return result;
    }

} // namespace database