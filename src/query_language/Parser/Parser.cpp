#include "Parser.h"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace database {

    std::unique_ptr<SQLStatement> Parser::parse(const std::string& sql) {
        Parser parser(sql);
        return parser.parseStatement();
    }

    Parser::Parser(const std::string& sql) : sql_(sql), pos_(0) {}

    std::unique_ptr<SQLStatement> Parser::parseStatement() {
        skipWhitespace();
        if (matchKeyword("CREATE")) {
            if (!matchKeyword("TABLE")) {
                throw std::runtime_error("Expected TABLE after CREATE");
            }
            return parseCreateTable();
        } else if (matchKeyword("INSERT")) {
            if (!matchKeyword("INTO")) {
                throw std::runtime_error("Expected INTO after INSERT");
            }
            return parseInsert();
        } else {
            throw std::runtime_error("Unsupported SQL statement.");
        }
    }

    std::unique_ptr<CreateTableStatement> Parser::parseCreateTable() {
        skipWhitespace();
        std::string tableName = parseIdentifier();
        skipWhitespace();
        
        if (pos_ >= sql_.size() || sql_[pos_] != '(') {
            throw std::runtime_error("Ожидалась '(' после названия таблицы.");
        }
        pos_++; // Пропускаем '('
        skipWhitespace();

        auto createStmt = std::make_unique<CreateTableStatement>();
        createStmt->tableName = tableName;

        while (pos_ < sql_.size()) {
            skipWhitespace();
            if (sql_[pos_] == ')') {
                pos_++; // Пропускаем ')'
                break;
            }

            // Парсим имя колонки
            std::string columnName = parseIdentifier();
            skipWhitespace();

            // Парсим тип данных
            std::string columnType = parseIdentifier();
            skipWhitespace();

            ColumnDefinition column;
            column.name = columnName;
            column.type = columnType;

            // Парсим атрибуты колонки
            while (pos_ < sql_.size()) {
                skipWhitespace();
                if (sql_[pos_] == ',' || sql_[pos_] == ')') {
                    break;
                }

                // Проверяем на наличие ключевых слов атрибутов
                if (matchKeyword("NOT NULL")) {
                    column.notNull = true;
                    std::cerr << "Parsed NOT NULL for column: " << column.name << std::endl;
                }
                else if (matchKeyword("PRIMARY KEY")) {
                    column.isPrimaryKey = true;
                    std::cerr << "Parsed PRIMARY KEY for column: " << column.name << std::endl;
                }
                else if (matchKeyword("UNIQUE")) {
                    column.isUnique = true;
                    std::cerr << "Parsed UNIQUE for column: " << column.name << std::endl;
                }
                else if (matchKeyword("DEFAULT")) {
                    skipWhitespace();
                    // Парсим значение по умолчанию
                    if (sql_[pos_] == '"') {
                        std::string defaultVal = "\"";
                        pos_++; // Пропускаем начальную кавычку
                        while (pos_ < sql_.size() && sql_[pos_] != '"') {
                            if (sql_[pos_] == '\\' && pos_ + 1 < sql_.size() && sql_[pos_ + 1] == '"') {
                                defaultVal += '\"';
                                pos_ += 2;
                            }
                            else {
                                defaultVal += sql_[pos_++];
                            }
                        }
                        if (pos_ >= sql_.size()) {
                            throw std::runtime_error("Незавершённый строковый литерал в DEFAULT значении.");
                        }
                        defaultVal += '"';
                        pos_++; // Пропускаем закрывающую кавычку
                        column.defaultValue = defaultVal;
                        std::cerr << "Parsed DEFAULT \"" << defaultVal << "\" for column: " << column.name << std::endl;
                    }
                    else {
                        // Парсим до следующей запятой или закрывающей скобки
                        std::string defaultVal;
                        while (pos_ < sql_.size() && sql_[pos_] != ',' && sql_[pos_] != ')') {
                            defaultVal += sql_[pos_++];
                        }
                        // Удаляем лишние пробелы
                        size_t start = defaultVal.find_first_not_of(" \t\n\r");
                        size_t end = defaultVal.find_last_not_of(" \t\n\r");
                        if (start != std::string::npos && end != std::string::npos) {
                            defaultVal = defaultVal.substr(start, end - start + 1);
                        }
                        else {
                            defaultVal = "";
                        }
                        column.defaultValue = defaultVal;
                        std::cerr << "Parsed DEFAULT " << defaultVal << " for column: " << column.name << std::endl;
                    }
                }
                else if (matchKeyword("FOREIGN KEY")) {
                    column.isForeignKey = true;
                    skipWhitespace();
                    if (sql_[pos_] != '(') {
                        throw std::runtime_error("Ожидалась '(' после FOREIGN KEY.");
                    }
                    pos_++; // Пропускаем '('
                    std::string refColumn = parseIdentifier();
                    skipWhitespace();
                    if (sql_[pos_] != ')') {
                        throw std::runtime_error("Ожидалась ')' после имени колонки FOREIGN KEY.");
                    }
                    pos_++; // Пропускаем ')'
                    skipWhitespace();
                    if (!matchKeyword("REFERENCES")) {
                        throw std::runtime_error("Ожидалось REFERENCES после FOREIGN KEY.");
                    }
                    skipWhitespace();
                    std::string refTable = parseIdentifier();
                    skipWhitespace();
                    if (sql_[pos_] != '(') {
                        throw std::runtime_error("Ожидалась '(' после названия таблицы в FOREIGN KEY.");
                    }
                    pos_++; // Пропускаем '('
                    std::string refTableColumn = parseIdentifier();
                    skipWhitespace();
                    if (sql_[pos_] != ')') {
                        throw std::runtime_error("Ожидалась ')' после имени внешней колонки в FOREIGN KEY.");
                    }
                    pos_++; // Пропускаем ')'
                    column.referencesTable = refTable;
                    column.referencesColumn = refTableColumn;
                    std::cerr << "Parsed FOREIGN KEY (" << refColumn << ") REFERENCES " 
                              << refTable << "(" << refTableColumn << ") for column: " 
                              << column.name << std::endl;
                }
                else if (matchKeyword("CHECK")) {
                    skipWhitespace();
                    if (sql_[pos_] != '(') {
                        throw std::runtime_error("Ожидалась '(' после CHECK.");
                    }
                    pos_++; // Пропускаем '('
                    // Парсим условие CHECK до закрывающей скобки
                    std::string checkCond;
                    int bracketCount = 1;
                    while (pos_ < sql_.size() && bracketCount > 0) {
                        if (sql_[pos_] == '(') bracketCount++;
                        else if (sql_[pos_] == ')') bracketCount--;
                        
                        if (bracketCount > 0) {
                            checkCond += sql_[pos_++];
                        }
                    }
                    if (bracketCount != 0) {
                        throw std::runtime_error("Незавершённое условие CHECK.");
                    }
                    pos_++; // Пропускаем ')'
                    column.checkCondition = checkCond;
                    std::cerr << "Parsed CHECK (" << checkCond << ") for column: " << column.name << std::endl;
                }
                else {
                    std::cerr << "Неизвестный атрибут колонки: ";
                    std::string unknownAttr = parseIdentifier();
                    std::cerr << unknownAttr << " at position " << pos_ << std::endl;
                    throw std::runtime_error("Неизвестный атрибут колонки: " + unknownAttr);
                }
            }

            createStmt->columns.push_back(column);
            skipWhitespace();

            // Если текущий символ - запятая, пропускаем её и продолжаем парсинг
            if (pos_ < sql_.size() && sql_[pos_] == ',') {
                pos_++;
                continue;
            }
            // Если текущий символ - закрывающая скобка, завершаем парсинг
            else if (pos_ < sql_.size() && sql_[pos_] == ')') {
                pos_++;
                break;
            }
            // В противном случае ожидаем запятую или закрывающую скобку
            else {
                std::cerr << "Ожидалась ',' или ')' после определения колонки. Текущая позиция: " 
                          << pos_ << ", символ: '" << sql_[pos_] << "'" << std::endl;
                throw std::runtime_error("Ожидалась ',' или ')' после определения колонки.");
            }
        }

        skipWhitespace();

        // Проверяем наличие точки с запятой
        if (pos_ >= sql_.size() || sql_[pos_] != ';') {
            throw std::runtime_error("Ожидалась ';' в конце CREATE TABLE запроса.");
        }
        pos_++; // Пропускаем ';'

        skipWhitespace();

        // Проверяем, что после точки с запятой нет лишних символов
        if (!isEnd()) {
            throw std::runtime_error("Лишние символы после ';' в CREATE TABLE запросе.");
        }

        return createStmt;
    }

    std::unique_ptr<InsertStatement> Parser::parseInsert() {
        skipWhitespace();

        std::string tableName = parseIdentifier();
        skipWhitespace();

        if (!matchKeyword("VALUES")) {
            throw std::runtime_error("Expected VALUES after table name.");
        }
        skipWhitespace();

        if (pos_ >= sql_.size() || sql_[pos_] != '(') {
            throw std::runtime_error("Expected '(' after VALUES.");
        }
        pos_++;
        skipWhitespace();

        auto insertStmt = std::make_unique<InsertStatement>();
        insertStmt->tableName = tableName;

        while (pos_ < sql_.size()) {
            skipWhitespace();

            std::string value;

            if (sql_[pos_] == '"') {
                value += '"';
                pos_++;
                while (pos_ < sql_.size()) {
                    if (sql_[pos_] == '\\' && pos_ + 1 < sql_.size() && sql_[pos_ + 1] == '"') {
                        value += '\"';
                        pos_ += 2;
                    }
                    else if (sql_[pos_] == '"') {
                        value += '"';
                        pos_++;
                        break;
                    }
                    else {
                        value += sql_[pos_++];
                    }
                }
                if (value.back() != '"') {
                    throw std::runtime_error("Unterminated string literal.");
                }
                insertStmt->values.push_back(value);
            }
            else {
                std::string expr;
                int localBracketCount = 0;
                while (pos_ < sql_.size()) {
                    char ch = sql_[pos_];
                    if (ch == '(') {
                        localBracketCount++;
                    }
                    else if (ch == ')') {
                        if (localBracketCount > 0) {
                            localBracketCount--;
                        }
                        else {
                            break;
                        }
                    }
                    else if (ch == ',' && localBracketCount == 0) {
                        break;
                    }

                    expr += ch;
                    pos_++;
                }

                size_t start = expr.find_first_not_of(" \t\n\r");
                size_t end = expr.find_last_not_of(" \t\n\r");
                if (start != std::string::npos && end != std::string::npos) {
                    expr = expr.substr(start, end - start +1);
                }
                else {
                    expr = "";
                }

                if (expr.empty()) {
                    throw std::runtime_error("Empty value in INSERT-query.");
                }

                insertStmt->values.push_back(expr);
            }

            skipWhitespace();

            if (pos_ < sql_.size()) {
                if (sql_[pos_] == ',') {
                    pos_++;
                    continue;
                }
                else if (sql_[pos_] == ')') {
                    pos_++;
                    break;
                }
                else {
                    throw std::runtime_error("Expected ',' or ')' in INSERT-query. Current position: " + std::to_string(pos_) + ", symbol: '" + std::string(1, sql_[pos_]) + "'");
                }
            }
            else {
                throw std::runtime_error("Unexpected end of input: missing ',' or ')'.");
            }
        }

        skipWhitespace();

        if (pos_ >= sql_.size() || sql_[pos_] != ';') {
            throw std::runtime_error("Unexpected end of input: missing ';'.");
        }
        pos_++;

        skipWhitespace();

        if (!isEnd()) {
            throw std::runtime_error("Extra symbols after ';'.");
        }

        return insertStmt;
    }

    void Parser::skipWhitespace() {
        while (pos_ < sql_.size() && std::isspace(sql_[pos_])) {
            pos_++;
        }
    }

    std::string Parser::parseIdentifier() {
        std::string identifier;
        skipWhitespace();
        
        if (pos_ >= sql_.size()) {
            throw std::runtime_error("Unexpected end of input while parsing identifier");
        }

        char firstChar = sql_[pos_];
        if (!(std::isalpha(firstChar) || firstChar == '_')) {
            throw std::runtime_error("Invalid identifier: must start with a letter or underscore");
        }

        identifier += sql_[pos_++];
        while (pos_ < sql_.size() && (std::isalnum(sql_[pos_]) || sql_[pos_] == '_')) {
            identifier += sql_[pos_++];
        }

        return identifier;
    }


    bool Parser::matchKeyword(const std::string& keyword) {
        skipWhitespace();
        size_t len = keyword.length();
        
        if (pos_ + len > sql_.size()) {
            return false;
        }

        if (sql_.substr(pos_, len) == keyword) {
            if (pos_ + len == sql_.size() || std::isspace(sql_[pos_ + len]) || 
                sql_[pos_ + len] == '(' || sql_[pos_ + len] == ')' || 
                sql_[pos_ + len] == ',' || sql_[pos_ + len] == ';') {
                pos_ += len;
                skipWhitespace();
                return true;
            }
        }
        return false;
    }

    bool Parser::isEnd() const {
        return pos_ >= sql_.size();
    }

} // namespace database 