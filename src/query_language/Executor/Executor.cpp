//
// Created by QUASARITY on 06.11.2024.
//

#include "Executor.h"

#include <memory>
#include <regex>
#include <string>
#include <variant>
#include <vector>

#include "../../Calculator/Calculator.h"
#include "../../database/Database/Database.h"
#include "../AST/SQLStatement.h"
#include "../Parser/Parser.h"
#include "../Result/Result.h"

namespace database {

Result Executor::execute(std::shared_ptr<SQLStatement> stmt) {
    Result result = {};
    try {
        calculator::Calculator calc;
        if (const auto *createStmt =
                dynamic_cast<const CreateTableStatement *>(stmt.get())) {
            m_database.createTable(createStmt->tableName, createStmt->columns);
        } else if (const auto *insertStmt =
                       dynamic_cast<const InsertStatement *>(stmt.get())) {
            const auto &table = m_database.getTable(insertStmt->tableName);
            const auto &columns = table.get_scheme();
            auto offsets = table.get_column_to_row_offset();

            if (!insertStmt->isMapFormat) {
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
                                    if (column.type == DataTypeName::INT)
                                        row[i] = 0;
                                    else if (column.type ==
                                             DataTypeName::DOUBLE)
                                        row[i] = 0.0;
                                    else if (column.type == DataTypeName::BOOL)
                                        row[i] = false;
                                    else if (column.type ==
                                             DataTypeName::STRING)
                                        row[i] = std::string("");
                                    else if (column.type ==
                                             DataTypeName::BYTEBUFFER)
                                        row[i] = {};
                                }
                            } else {
                                auto value =
                                    calc.evaluate(insertStmt->values[i]);
                                if ((column.type == DataTypeName::INT &&
                                     !std::holds_alternative<int>(value)) ||
                                    (column.type == DataTypeName::DOUBLE &&
                                     !std::holds_alternative<double>(value)) ||
                                    (column.type == DataTypeName::BOOL &&
                                     !std::holds_alternative<bool>(value)) ||
                                    (column.type == DataTypeName::STRING &&
                                     !std::holds_alternative<std::string>(
                                         value)) ||
                                    (column.type == DataTypeName::BYTEBUFFER &&
                                     !std::holds_alternative<bytebuffer>(
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
            } else {
                std::vector<DBType> row(columns.size());
                std::unordered_set<std::string> providedColumns;

                for (const auto &[columnName, value] :
                     insertStmt->columnValuePairs) {
                    if (!offsets.count(columnName)) {
                        throw std::runtime_error("Unknown column: " +
                                                 columnName);
                    }
                    if (!providedColumns.insert(columnName).second) {
                        throw std::runtime_error(
                            "Duplicate column in assignment: " + columnName);
                    }
                }

                for (const auto &column : columns) {
                    if (!column.hasDefault && !column.isAutoIncrement &&
                        !providedColumns.count(column.name)) {
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
                            if (column.type == DataTypeName::INT)
                                row[i] = 0;
                            else if (column.type == DataTypeName::DOUBLE)
                                row[i] = 0.0;
                            else if (column.type == DataTypeName::BOOL)
                                row[i] = false;
                            else if (column.type == DataTypeName::STRING)
                                row[i] = std::string("");
                            else if (column.type == DataTypeName::BYTEBUFFER)
                                row[i] = {};
                        }
                    }
                }

                for (const auto &[columnName, valueExpr] :
                     insertStmt->columnValuePairs) {
                    size_t columnIndex = offsets[columnName];
                    const auto &column = columns[columnIndex];

                    try {
                        auto value = calc.evaluate(valueExpr);
                        if ((column.type == DataTypeName::INT &&
                             !std::holds_alternative<int>(value)) ||
                            (column.type == DataTypeName::DOUBLE &&
                             !std::holds_alternative<double>(value)) ||
                            (column.type == DataTypeName::BOOL &&
                             !std::holds_alternative<bool>(value)) ||
                            (column.type == DataTypeName::STRING &&
                             !std::holds_alternative<std::string>(value)) ||
                            (column.type == DataTypeName::BYTEBUFFER &&
                             !std::holds_alternative<bytebuffer>(value))) {
                            throw std::runtime_error(
                                "Type mismatch for column " + columnName);
                        }
                        row[columnIndex] = value;
                    } catch (const std::exception &e) {
                        throw std::runtime_error(
                            "Error processing value for column " + columnName +
                            ": " + e.what());
                    }
                }

                m_database.insertInto(insertStmt->tableName, row);
            }
        } else if (const auto *selectStmt =
                       dynamic_cast<const SelectStatement *>(stmt.get())) {
            auto table = m_database.getTable(selectStmt->tableName);
            Table foreignTable;
            if (!selectStmt->foreignTableName.empty()) {
                foreignTable =
                    m_database.getTable(selectStmt->foreignTableName);
            }

            std::vector<ResultRowType> result_rows;

            for (const auto &column : selectStmt->columnData) {
                if (column.name == "*") {
                    break;
                }
                if (column.table.empty()) {
                    if (!table.get_column_to_row_offset().count(column.name)) {
                        throw std::invalid_argument(
                            "Invalid selector: " + column.name + ".");
                    }
                } else {
                    if (m_database.hasTable(column.table)) {
                        if (column.table == selectStmt->tableName) {
                            if (!table.get_column_to_row_offset().count(
                                    column.name)) {
                                throw std::invalid_argument(
                                    "Invalid selector: " + column.table + "." +
                                    column.name + ".");
                            }
                        } else if (column.table ==
                                   selectStmt->foreignTableName) {
                            if (!foreignTable.get_column_to_row_offset().count(
                                    column.name)) {
                                throw std::invalid_argument(
                                    "Invalid selector: " + column.table + "." +
                                    column.name + ".");
                            }
                        } else {
                            throw std::runtime_error(
                                "Unknown table in selector: " + column.table);
                        }
                    } else {
                        throw std::runtime_error("Unknown table in selector: " +
                                                 column.table);
                    }
                }
            }

            if (selectStmt->foreignTableName.empty()) {
                // handling select without join
                auto filter_predicate = [table, foreignTable, selectStmt,
                                         calc](const std::vector<DBType> &row) {
                    std::unordered_map<std::string, std::string> row_values =
                        {};

                    for (const auto &[name, index] :
                         table.get_column_to_row_offset()) {
                        row_values[name] = dBTypeToString(row[index]);
                    }

                    return calculator::safeGet<bool>(
                        calc.evaluate(selectStmt->predicate, row_values));
                };

                auto rows = !selectStmt->predicate.empty()
                                ? table.filter(filter_predicate)
                                : table.get_rows();

                for (const auto &column : rows) {
                    std::unordered_map<std::string, DBType> row = {};
                    if (selectStmt->columnData[0].name == "*") {
                        for (const auto &[name, index] :
                             table.get_column_to_row_offset()) {
                            row[name] = column[index];
                        }
                    } else {
                        for (const auto &columnItem : selectStmt->columnData) {
                            row[columnItem.name] =
                                column[table.get_column_to_row_offset()
                                           [columnItem.name]];
                        }
                    }
                    result_rows.emplace_back(row);
                }
            } else {
                // handling select with join
                auto rows = table.get_rows();

                auto foreignRows = foreignTable.get_rows();

                for (const auto &column : rows) {
                    for (const auto &foreignColumn : foreignRows) {
                        std::unordered_map<std::string, std::string>
                            row_values = {};

                        for (const auto &[name, index] :
                             table.get_column_to_row_offset()) {
                            row_values[selectStmt->tableName + '.' + name] =
                                dBTypeToString(column[index]);
                        }
                        for (const auto &[name, index] :
                             foreignTable.get_column_to_row_offset()) {
                            row_values[selectStmt->foreignTableName + '.' +
                                       name] =
                                dBTypeToString(foreignColumn[index]);
                        }

                        if (!calculator::safeGet<bool>(calc.evaluate(
                                selectStmt->joinPredicate, row_values)) ||
                            !calculator::safeGet<bool>(calc.evaluate(
                                selectStmt->predicate, row_values))) {
                            continue;
                        }

                        std::unordered_map<std::string, DBType> row = {};
                        if (selectStmt->columnData[0].name == "*") {
                            for (const auto &[name, index] :
                                 table.get_column_to_row_offset()) {
                                row[selectStmt->tableName + '.' + name] =
                                    column[index];
                            }
                            for (const auto &[name, index] :
                                 foreignTable.get_column_to_row_offset()) {
                                row[selectStmt->foreignTableName + '.' + name] =
                                    foreignColumn[index];
                            }
                        } else {
                            for (const auto &columnItem :
                                 selectStmt->columnData) {
                                if (columnItem.table == selectStmt->tableName) {
                                    row[columnItem.table + '.' +
                                        columnItem.name] =
                                        column[table.get_column_to_row_offset()
                                                   [columnItem.name]];
                                } else {
                                    row[columnItem.table + '.' +
                                        columnItem.name] = foreignColumn
                                        [foreignTable.get_column_to_row_offset()
                                             [columnItem.name]];
                                }
                            }
                        }
                        result_rows.emplace_back(row);
                    }
                }
            }

            result = Result(std::move(result_rows));
        } else if (const auto *updateStmt =
                       dynamic_cast<const UpdateStatement *>(stmt.get())) {
            Table &table = m_database.getTable(updateStmt->tableName);

            // check if columns are valid
            for (const auto &[columnData, value] : updateStmt->newValues) {
                if (!table.get_column_to_row_offset().count(columnData.name)) {
                    throw std::invalid_argument(
                        "Invalid value name: " + columnData.name + ".");
                }

                auto column =
                    table.get_scheme()
                        [table.get_column_to_row_offset()[columnData.name]];

                if (column.isAutoIncrement) {
                    throw std::invalid_argument(
                        "Cannot update autoincrement column: " +
                        columnData.name + ".");
                }

                if (column.isKey) {
                    throw std::invalid_argument(
                        "Cannot update key column: " + columnData.name + ".");
                }

                if (column.isUnique) {
                    throw std::invalid_argument(
                        "Cannot update unique column: " + columnData.name +
                        ".");
                }
            }

            auto updater = [table, updateStmt, calc](std::vector<DBType> &row) {
                std::unordered_map<std::string, std::string> row_values = {};
                for (const auto &[name, index] :
                     table.get_column_to_row_offset()) {
                    row_values[name] = dBTypeToString(row[index]);
                }

                for (const auto &[columnItem, value] : updateStmt->newValues) {
                    row[table.get_column_to_row_offset()[columnItem.name]] =
                        calc.evaluate(value, row_values);
                }
            };

            if (updateStmt->predicate.empty()) {
                table.update_many(
                    updater,
                    []([[maybe_unused]] const std::vector<DBType> &row) {
                        return true;
                    });
            } else {
                auto filter_predicate = [table, updateStmt,
                                         calc](const std::vector<DBType> &row) {
                    std::unordered_map<std::string, std::string> row_values =
                        {};
                    for (const auto &[name, index] :
                         table.get_column_to_row_offset()) {
                        auto value = row[index];
                        row_values[name] = dBTypeToString(row[index]);
                    }
                    try {
                        auto result = calculator::safeGet<bool>(
                            calc.evaluate(updateStmt->predicate, row_values));
                        return result;
                    } catch (const std::exception &e) {
                        throw;
                    }
                };

                table.update_many(updater, filter_predicate);
            }
        } else if (const auto *insertStmt =
                       dynamic_cast<const DeleteStatement *>(stmt.get())) {
            Table &table = m_database.getTable(insertStmt->tableName);

            if (insertStmt->predicate.empty()) {
                table.drop_rows();
            } else {
                // build the predicate
                auto filter_predicate = [table, insertStmt,
                                         calc](const std::vector<DBType> &row) {
                    std::unordered_map<std::string, std::string> row_values =
                        {};
                    for (const auto &[name, index] :
                         table.get_column_to_row_offset()) {
                        row_values[name] = dBTypeToString(row[index]);
                    }
                    return calculator::safeGet<bool>(
                        calc.evaluate(insertStmt->predicate, row_values));
                };

                table.remove_many(filter_predicate);
            }
        } else if (const auto *createIndexStmt =
                    dynamic_cast<const CreateIndexStatement *>(stmt.get())) {
            Table &table = m_database.getTable(createIndexStmt->tableName);
            std::string indexTypeStr = (createIndexStmt->indexType == IndexType::ORDERED) ? "ordered" : "unordered";
            table.createIndex(indexTypeStr, createIndexStmt->columns);
        } else {
            throw std::runtime_error("Unsupported SQL statement.");
        }
    } catch (const std::exception &e) {
        result = Result::errorResult(std::string(e.what()));
    }
    return result;
}

Result Executor::execute(const std::string &sql) {
    try {
        std::shared_ptr<SQLStatement> stmt = Parser::parse(sql);
        return Executor::execute(stmt);
    } catch (const std::exception &e) {
        return Result::errorResult(std::string(e.what()));
    }
}

}  // namespace database
