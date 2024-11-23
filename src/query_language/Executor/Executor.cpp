//
// Created by QUASARITY on 06.11.2024.
//

#include "Executor.h"
#include "../AST/SQLStatement.h"
#include "../../database/Database/Database.h"
#include "../../Calculator/Calculator.h"
#include <iostream>

namespace database {

    void Executor::execute(const SQLStatement& stmt) {
        if (const auto* createStmt = dynamic_cast<const CreateTableStatement*>(&stmt)) {
            std::cout << "Executing " << createStmt->toString() << std::endl;
            m_database.createTable(createStmt->tableName, createStmt->columns);
            std::cout << "Table '" << createStmt->tableName << "' created successfully." << std::endl;
        } else if (const auto* insertStmt = dynamic_cast<const InsertStatement*>(&stmt)) {
            std::cout << "Executing " << insertStmt->toString() << std::endl;
            std::vector<DBType> row;
            calculator::Calculator calc;
            for (const auto& valStr : insertStmt->values) {
                if (valStr.front() == '"' && valStr.back() == '"') {
                    row.emplace_back(valStr.substr(1, valStr.size() - 2));
                }
                else if (valStr == "true" || valStr == "false") {
                    row.emplace_back(valStr == "true");
                }
                else if (valStr.find('.') != std::string::npos) {
                    row.emplace_back(std::stod(valStr));
                }
                else {
                    try {
                        auto result = calc.evaluate(valStr);
                        if (std::holds_alternative<int>(result)) {
                            row.emplace_back(std::get<int>(result));
                        } else if (std::holds_alternative<double>(result)) {
                            row.emplace_back(std::get<double>(result));
                        } else {
                            throw std::runtime_error("Unsupported type in expression.");
                        }
                    } catch (...) {
                        row.emplace_back(std::stoi(valStr));
                    }
                }
            }
            m_database.insertInto(insertStmt->tableName, row);
            std::cout << "Data inserted into '" << insertStmt->tableName << "' successfully." << std::endl;
        } else {
            std::cerr << "Unsupported SQL statement." << std::endl;
        }
    }

} // namespace database