#include "Parser.h"
#include <algorithm>

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
        
        if (sql_[pos_] != '(') {
            throw std::runtime_error("Expected '(' after table name.");
        }
        pos_++;
        skipWhitespace();

        auto createStmt = std::make_unique<CreateTableStatement>();
        createStmt->tableName = tableName;

        while (!isEnd()) {
            skipWhitespace();
            
            if (sql_[pos_] == ')') {
                pos_++;
                break;
            }

            std::string columnName = parseIdentifier();
            skipWhitespace();

            std::string columnType;
            while (pos_ < sql_.size() && 
                   sql_[pos_] != ',' && 
                   sql_[pos_] != ')' && 
                   !std::isspace(sql_[pos_])) {
                columnType += sql_[pos_++];
            }

            if (columnType.empty()) {
                throw std::runtime_error("Expected column type after column name");
            }

            createStmt->columns.push_back(ColumnDefinition{columnName, columnType});

            skipWhitespace();
            if (sql_[pos_] == ',') {
                pos_++;
                continue;
            }
        }

        skipWhitespace();
        if (pos_ >= sql_.size() || sql_[pos_] != ';') {
            throw std::runtime_error("Expected ';' at the end of CREATE TABLE statement.");
        }
        pos_++;

        return createStmt;
    }

    std::unique_ptr<InsertStatement> Parser::parseInsert() {
        skipWhitespace();

        std::string tableName = parseIdentifier();
        skipWhitespace();

        if (!matchKeyword("VALUES")) {
            throw std::runtime_error("Ожидалось VALUES после названия таблицы.");
        }
        skipWhitespace();

        if (pos_ >= sql_.size() || sql_[pos_] != '(') {
            throw std::runtime_error("Ожидалась '(' после VALUES.");
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
                    throw std::runtime_error("Незавершённый строковый литерал.");
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

    std::string Parser::parseToken() {
        std::string token;
        skipWhitespace();
        
        while (pos_ < sql_.size() && !std::isspace(sql_[pos_]) && sql_[pos_] != ',' && 
               sql_[pos_] != ')' && sql_[pos_] != ';' && sql_[pos_] != '"') {
            token += sql_[pos_++];
        }

        if (token.empty()) {
            throw std::runtime_error("Empty token");
        }

        return token;
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