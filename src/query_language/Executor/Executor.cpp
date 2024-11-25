//
// Created by QUASARITY on 06.11.2024.
//

#include "Executor.h"
#include <iostream>
#include <regex>
#include "../../Calculator/Calculator.h"
#include "../../database/Database/Database.h"
#include "../AST/SQLStatement.h"
#include "../Result/Result.h"

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

Result Executor::execute(const SQLStatement &stmt) {
    Result result = {};
    try {
        calculator::Calculator calc;
        if (const auto *createStmt =
                dynamic_cast<const CreateTableStatement *>(&stmt)) {
            std::cout << "Executing CREATE TABLE statement" << std::endl;
            m_database.createTable(createStmt->tableName, createStmt->columns);
        } else if (const auto *insertStmt =
                       dynamic_cast<const InsertStatement *>(&stmt)) {
            std::cout << "Executing INSERT statement" << std::endl;
            const auto &table = m_database.getTable(insertStmt->tableName);
            const auto &columns = table.get_scheme();
            auto offsets = table.get_column_to_row_offset();
Result Executor::execute(std::shared_ptr<SQLStatement> stmt) {
    Result result = {};
    try {
        calculator::Calculator calc;
        if (const auto *createStmt = dynamic_cast<const CreateTableStatement *>(stmt.get())) {
            m_database.createTable(createStmt->tableName, createStmt->columns);
        } else if (const auto *insertStmt =
                       dynamic_cast<const InsertStatement *>(stmt.get())) {
            std::vector<DBType> row;
            const auto &table = m_database.getTable(insertStmt->tableName);
            const auto &columns = table.get_scheme();

            if (insertStmt->values.size() > columns.size()) {
                throw std::runtime_error("Too many values provided.");
            }
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
                            if (insertStmt->values[i].empty()) {
                                if (column.isAutoIncrement) {
                                    row[i] = 0;
                                } else if (column.hasDefault) {
                                    row[i] = calc.evaluate(column.defaultValue);
                                } else {
                                    throw std::runtime_error(
                                        "No default value for column " +
                                        column.name);
                                }
                            } else if (insertStmt->values[i] == "NULL") {
                                if (column.isAutoIncrement) {
                                    row[i] = 0;
                                } else if (column.hasDefault) {
                                    row[i] = calc.evaluate(column.defaultValue);
                                } else {
                                    if (column.type == "INT")
                                        row[i] = 0;
                                    else if (column.type == "DOUBLE")
                                        row[i] = 0.0;
                                    else if (column.type == "BOOL")
                                        row[i] = false;
                                    else if (column.type == "VARCHAR")
                                        row[i] = std::string("");
                                }
                            } else {
                                auto value =
                                    calc.evaluate(insertStmt->values[i]);
                                if ((column.type == "INT" &&
                                     !std::holds_alternative<int>(value)) ||
                                    (column.type == "DOUBLE" &&
                                     !std::holds_alternative<double>(value)) ||
                                    (column.type == "BOOL" &&
                                     !std::holds_alternative<bool>(value)) ||
                                    (column.type == "VARCHAR" &&
                                     !std::holds_alternative<std::string>(
                                         value))) {
                                    throw std::runtime_error(
                                        "Type mismatch for column " +
                                        column.name);
                                }
                                row[i] = value;
                            }
                        } else if (column.isAutoIncrement) {
                            row[i] = 0;
                        } else if (column.hasDefault) {
                            row[i] = calc.evaluate(column.defaultValue);
                        } else {
                            throw std::runtime_error(
                                "No value provided for column " + column.name);
                        }
                    } catch (const std::exception &e) {
                        throw std::runtime_error(
                            "Error processing value for column " + column.name +
                            ": " + e.what());
                    }
                }
                m_database.insertInto(insertStmt->tableName, row);
            } else if (const auto *insertStmt = dynamic_cast<const SelectStatement *>(stmt.get())) {
            auto table = m_database.getTable(insertStmt->tableName);
            } else {
                std::cout << "Processing column assignments format"
                          << std::endl;
                std::vector<DBType> row(columns.size());
                std::unordered_set<std::string> providedColumns;

                for (const auto &[columnName, value] :
                     insertStmt->columnValuePairs) {
                    if (!offsets.count(columnName)) {
                        std::cout << "Error: Unknown column: " << columnName
                                  << std::endl;
                        throw std::runtime_error("Unknown column: " +
                                                 columnName);
                    }
                    if (!providedColumns.insert(columnName).second) {
                        std::cout << "Error: Duplicate column in assignment: "
                                  << columnName << std::endl;
                        throw std::runtime_error(
                            "Duplicate column in assignment: " + columnName);
                    }
                }

                for (const auto &column : columns) {
                    if (!column.hasDefault && !column.isAutoIncrement &&
                        !providedColumns.count(column.name)) {
                        std::cout
                            << "Error: Missing required column: " << column.name
                            << std::endl;
                        throw std::runtime_error(
                            "Missing value for required column: " +
                            column.name);
                    }
                }

                for (size_t i = 0; i < columns.size(); ++i) {
                    const auto &column = columns[i];
                    if (!providedColumns.count(column.name)) {
                        if (column.isAutoIncrement) {
                            row[i] = 0;
                        } else if (column.hasDefault) {
                            try {
                                row[i] = calc.evaluate(column.defaultValue);
                            } catch (const std::exception &e) {
                                throw std::runtime_error(
                                    "Error evaluating default value for "
                                    "column " +
                                    column.name + ": " + e.what());
                            }
                        } else {
                            if (column.type == "INT")
                                row[i] = 0;
                            else if (column.type == "DOUBLE")
                                row[i] = 0.0;
                            else if (column.type == "BOOL")
                                row[i] = false;
                            else if (column.type == "VARCHAR")
                                row[i] = std::string("");
                        }
                    }
                }

                for (const auto &[columnName, valueExpr] :
                     insertStmt->columnValuePairs) {
                    size_t columnIndex = offsets[columnName];
                    const auto &column = columns[columnIndex];

            // check if columns are valid
            for (const auto &column_name : insertStmt->columnNames) {
                if (!table.get_column_to_row_offset().count(column_name)) {
                    throw std::invalid_argument(
                        "Invalid selector: " + column_name + ".");
                }
            }
                    try {
                        auto value = calc.evaluate(valueExpr);
                        if ((column.type == "INT" &&
                             !std::holds_alternative<int>(value)) ||
                            (column.type == "DOUBLE" &&
                             !std::holds_alternative<double>(value)) ||
                            (column.type == "BOOL" &&
                             !std::holds_alternative<bool>(value)) ||
                            (column.type == "VARCHAR" &&
                             !std::holds_alternative<std::string>(value))) {
                            std::cout << "Error: Type mismatch for column "
                                      << columnName << std::endl;
                            throw std::runtime_error(
                                "Type mismatch for column " + columnName);
                        }
                        row[columnIndex] = value;
                    } catch (const std::exception &e) {
                        std::cout << "Error evaluating " << columnName << ": "
                                  << e.what() << std::endl;
                        throw std::runtime_error(
                            "Error processing value for column " + columnName +
                            ": " + e.what());
                    }
                }

                m_database.insertInto(insertStmt->tableName, row);
            }
        } else if (const auto *selectStmt =
                       dynamic_cast<const SelectStatement *>(&stmt)) {
            auto table = m_database.getTable(selectStmt->tableName);

            for (const auto &column_name : selectStmt->columnNames) {
                if (!table.get_column_to_row_offset().count(column_name)) {
                    throw std::invalid_argument(
                        "Invalid selector: " + column_name + ".");
                }
            }

            // build the predicate
            auto filter_predicate = [table, insertStmt,
                                     calc](const std::vector<DBType> &row) {
                std::unordered_map<std::string, std::string> row_values = {};
                for (const auto &[name, index] :
                     table.get_column_to_row_offset()) {
                    row_values[name] = dBTypeToString(row[index]);
                }
                return calculator::safeGet<bool>(
                    calc.evaluate(insertStmt->predicate, row_values));
            };

            std::vector<ResultRowType> result_rows;

            for (const auto &column : !insertStmt->predicate.empty()
                                          ? table.filter(filter_predicate)
                                          : table.get_rows()) {
                std::unordered_map<std::string, DBType> row = {};
                if (insertStmt->columnNames[0] == "*") {
                    for (const auto &[name, index] :
                         table.get_column_to_row_offset()) {
                        row[name] = column[index];
                    }
                } else {
                    for (const auto &column_name : insertStmt->columnNames) {
                        row[column_name] = column
                            [table.get_column_to_row_offset()[column_name]];
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
            result = Result(std::move(result_rows));
        } else if (const auto *insertStmt =
                       dynamic_cast<const UpdateStatement *>(stmt.get())) {
            Table &table = m_database.getTable(insertStmt->tableName);

            // check if columns are valid
            for (const auto &[key, value] : insertStmt->newValues) {
                if (!table.get_column_to_row_offset().count(key)) {
                    throw std::invalid_argument("Invalid value name: " + key +
                                                ".");
                }
            }

            // build the predicate
            auto filter_predicate = [table, insertStmt,
                                     calc](const std::vector<DBType> &row) {
                std::unordered_map<std::string, std::string> row_values = {};
                for (const auto &[name, index] :
                     table.get_column_to_row_offset()) {
                    row_values[name] = dBTypeToString(row[index]);
                }
                return calculator::safeGet<bool>(
                    calc.evaluate(insertStmt->predicate, row_values));
            };

            auto updater = [table, insertStmt, calc](std::vector<DBType> &row) {
                for (const auto &[key, value] : insertStmt->newValues) {
                    row[table.get_column_to_row_offset()[key]] =
                        calc.evaluate(value);
                }
            };

            if (insertStmt->predicate.empty()) {
                table.update_many(updater, [table, insertStmt, calc](
                                               const std::vector<DBType> &row) {
                    return true;
                });
            } else {
                table.update_many(updater, filter_predicate);
            }
        } else {
            throw std::runtime_error("Unsupported SQL statement.");
        }
    } catch (const std::exception &e) {
        result = Result::errorResult(std::string(e.what()));
    }
    return result;
}

}  // namespace database