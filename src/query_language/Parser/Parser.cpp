#include "Parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <locale>
#include <stdexcept>

namespace database {

std::shared_ptr<SQLStatement> Parser::parse(const std::string& sql) {
    Parser parser(sql);
    return parser.parseStatement();
}

Parser::Parser(const std::string& sql) : sql_(sql), pos_(0) {}

std::unordered_map<std::string, std::string> Parser::parseAssignValues() {
    std::unordered_map<std::string, std::string> columnValuePairs;
    bool firstColumn = true;
    while (pos_ < sql_.size()) {
        skipWhitespace();

        if (sql_[pos_] == ')') {
            pos_++;
            break;
        }

        std::string columnName = parseIdentifier();
        skipWhitespace();

        if (pos_ >= sql_.size() || sql_[pos_] != '=') {
            std::cout << "Error: Expected '=' after column name: " << columnName
                      << " at position " << pos_ << std::endl;
            throw std::runtime_error("Expected '=' after column name: " +
                                     columnName);
        }
        pos_++;
        skipWhitespace();

        std::string value;
        if (sql_[pos_] == '"') {
            value = parseStringLiteral();
        } else {
            value = parseExpression();
        }

        columnValuePairs[columnName] = value;
        firstColumn = false;

        skipWhitespace();
        if (pos_ >= sql_.size()) {
            std::cout
                << "Error: Unexpected end of input after value at position "
                << pos_ << std::endl;
            throw std::runtime_error("Unexpected end of input after value");
        }
        if (sql_[pos_] == ',') {
            pos_++;
            skipWhitespace();
            if (pos_ >= sql_.size() ||
                (!std::isalpha(sql_[pos_]) && sql_[pos_] != '_')) {
                std::cout
                    << "Error: Expected column name after comma at position "
                    << pos_ << std::endl;
                throw std::runtime_error("Expected column name after comma");
            }
            continue;
        }
        if (sql_[pos_] == ')') {
            pos_++;
            break;
        }
        std::cout << "Error: Expected ',' or ')' after value at position "
                  << pos_ << std::endl;
        throw std::runtime_error("Expected ',' or ')' after value");
    }
    return columnValuePairs;
}

std::unordered_map<std::string, std::string> Parser::parseAssign(
    const std::string& sql) {
    Parser parser(sql);
    return parser.parseAssignValues();
}

std::shared_ptr<SQLStatement> Parser::parseStatement() {
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
    } else if (matchKeyword("SELECT")) {
        return parseSelect();
    } else if (matchKeyword("UPDATE")) {
        return parseUpdate();
    } else {
        throw std::runtime_error("Unsupported SQL statement.");
    }
}

std::shared_ptr<CreateTableStatement> Parser::parseCreateTable() {
    skipWhitespace();
    std::string tableName = parseIdentifier();
    skipWhitespace();

    if (pos_ >= sql_.size() || sql_[pos_] != '(') {
        throw std::runtime_error("Expected '(' after table name.");
    }
    pos_++;
    skipWhitespace();

    auto createStmt = std::make_shared<CreateTableStatement>();
    createStmt->tableName = tableName;

    while (pos_ < sql_.size()) {
        skipWhitespace();

        if (sql_[pos_] == ')') {
            pos_++;
            break;
        }

        std::string columnName = parseIdentifier();
        skipWhitespace();

        std::string columnType = parseIdentifier();
        skipWhitespace();

        ColumnDefinition column;
        column.name = columnName;
        column.type = columnType;

        while (pos_ < sql_.size()) {
            skipWhitespace();
            if (sql_[pos_] == ',' || sql_[pos_] == ')') {
                break;
            } else if (matchKeyword("UNIQUE")) {
                column.isUnique = true;
            } else if (matchKeyword("AUTOINCREMENT")) {
                if (column.type != "INT") {
                    throw std::runtime_error(
                        "AUTOINCREMENT is only applicable to integer columns.");
                }
                column.isAutoIncrement = true;
            } else if (matchKeyword("KEY")) {
                column.isUnique = true;
                column.isKey = true;
            } else if (matchKeyword("DEFAULT")) {
                column.hasDefault = true;
                skipWhitespace();
                if (sql_[pos_] == '"') {
                    column.defaultValue = parseStringLiteral();
                } else {
                    column.defaultValue = parseExpression();
                }
            } else {
                std::string unknownAttr = parseIdentifier();
                throw std::runtime_error("Unknown column attribute: " +
                                         unknownAttr);
            }
        }

        createStmt->columns.push_back(column);

        skipWhitespace();
        if (sql_[pos_] == ',') {
            pos_++;
            continue;
        }
    }

    skipWhitespace();
    if (pos_ >= sql_.size() || sql_[pos_] != ';') {
        throw std::runtime_error(
            "Expected ';' at the end of CREATE TABLE statement.");
    }
    pos_++;

    return createStmt;
}

std::shared_ptr<InsertStatement> Parser::parseInsert() {
    skipWhitespace();

    std::string tableName = parseIdentifier();
    skipWhitespace();

    auto insertStmt = std::make_unique<InsertStatement>();
    insertStmt->tableName = tableName;
    bool flagValues = matchKeyword("VALUES");
    if (flagValues) {
        skipWhitespace();
    }
    if (pos_ >= sql_.size() || sql_[pos_] != '(') {
        throw std::runtime_error("Expected '(' after VALUES.");
    }
    pos_++;
    skipWhitespace();

    if (flagValues) {
        std::vector<std::string> values;
        bool expectValue = true;

        while (pos_ < sql_.size()) {
            skipWhitespace();

            if (sql_[pos_] == ')') {
                if (expectValue) {
                    values.push_back("");
                }
                pos_++;
                break;
            }

            if (sql_[pos_] == ',') {
                if (expectValue) {
                    values.push_back("");
                }
                pos_++;
                expectValue = true;
                continue;
            }

            if (!expectValue) {
                throw std::runtime_error(
                    "Expected ',' or ')' after value in VALUES clause");
            }

            std::string value;
            if (sql_[pos_] == '"') {
                value = parseStringLiteral();
            } else {
                value = parseExpression();
            }

            if (!value.empty()) {
                values.push_back(value);
            } else {
                values.push_back("");
            }
            expectValue = false;

            skipWhitespace();
        }

        insertStmt->values = values;
    } else {
        insertStmt->isMapFormat = true;
        insertStmt->columnValuePairs = parseAssignValues();
    }

    skipWhitespace();
    if (pos_ >= sql_.size() || sql_[pos_] != ';') {
        throw std::runtime_error("Expected ';' at end of INSERT statement");
    }
    pos_++;

    return insertStmt;
}

std::shared_ptr<SelectStatement> Parser::parseSelect() {
    auto selectStmt = std::make_shared<SelectStatement>();

    skipWhitespace();
    std::vector<std::string> columns = {};

    do {
        columns.push_back(parseIdentifier());
    } while (sql_[pos_++] == ',');

    selectStmt->columnNames = columns;

    skipWhitespace();

    if (!matchKeyword("FROM")) {
        throw std::runtime_error(
            "Expected FROM after column list in SELECT statement.");
    }

    std::string tableName = parseIdentifier();
    selectStmt->tableName = tableName;

    skipWhitespace();

    if (matchKeyword("WHERE")) {
        std::string predicate;
        std::string current_token = parseToken();

        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            predicate += current_token + " ";
            current_token = parseToken();
        }
        predicate += current_token;
        selectStmt->predicate = predicate;
    }

    return selectStmt;
}

std::shared_ptr<UpdateStatement> Parser::parseUpdate() {
    auto selectStmt = std::make_unique<UpdateStatement>();
    skipWhitespace();

    selectStmt->tableName = parseIdentifier();

    if (!matchKeyword("SET")) {
        throw std::runtime_error(
            "Expected SET after table name in UPDATE statement.");
    }

    pos_++;
    selectStmt->newValues = parseAssignValues();
    skipWhitespace();

    if (matchKeyword("WHERE")) {
        std::string predicate;
        std::string current_token = parseToken();

        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            predicate += current_token + " ";
            current_token = parseToken();
        }
        predicate += current_token;
        selectStmt->predicate = predicate;
    }

    return selectStmt;
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
        throw std::runtime_error(
            "Unexpected end of input while parsing identifier");
    }

    char firstChar = sql_[pos_];
    if (!(std::isalpha(firstChar) || firstChar == '_' || firstChar == '*')) {
        throw std::runtime_error(
            "Invalid identifier: must start with a letter or underscore");
    }

    identifier += sql_[pos_++];
    while (pos_ < sql_.size() &&
           (std::isalnum(sql_[pos_]) || sql_[pos_] == '_')) {
        identifier += sql_[pos_++];
    }

    return identifier;
}

std::string Parser::parseToken() {
    std::string token;
    skipWhitespace();

    while (pos_ < sql_.size() && !std::isspace(sql_[pos_]) &&
           sql_[pos_] != ',' && sql_[pos_] != ')' && sql_[pos_] != ';' &&
           sql_[pos_] != '"') {
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

bool Parser::matchCharacter(char expected) {
    if (pos_ < sql_.size() && sql_[pos_] == expected) {
        pos_++;
        skipWhitespace();
        return true;
    }
    return false;
}

bool Parser::isEnd() const { return pos_ >= sql_.size(); }

std::string Parser::trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() &&
           std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    if (start == s.size()) {
        return "";
    }

    size_t end = s.size() - 1;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end]))) {
        end--;
    }

    return s.substr(start, end - start + 1);
}

bool Parser::isBooleanLiteral(const std::string& expr) {
    return expr == "true" || expr == "false";
}

std::string Parser::parseStringLiteral() {
    std::string value = "\"";
    pos_++;

    while (pos_ < sql_.size() && sql_[pos_] != '"') {
        if (sql_[pos_] == '\\' && pos_ + 1 < sql_.size()) {
            value += sql_[pos_++];
        }
        value += sql_[pos_++];
    }

    if (pos_ >= sql_.size() || sql_[pos_] != '"') {
        throw std::runtime_error("Unterminated string literal");
    }

    value += '"';
    pos_++;
    return value;
}

std::string Parser::parseExpression() {
    std::string value;
    int brackets = 0;

    std::cout << "Starting parseExpression at position " << pos_
              << ", current char: '" << sql_[pos_] << "'" << std::endl;
    std::cout << "Remaining SQL: " << sql_.substr(pos_) << std::endl;

    skipWhitespace();
    if (sql_[pos_] == ',' || sql_[pos_] == ')') {
        std::cout << "Found empty value" << std::endl;
        return "";
    }

    while (pos_ < sql_.size()) {
        char c = sql_[pos_];
        std::cout << "Processing char: '" << c << "' at position " << pos_
                  << std::endl;

        if (c == '(') {
            std::cout << "Found opening bracket, current depth: " << brackets
                      << std::endl;
            brackets++;
        } else if (c == ')') {
            std::cout << "Found closing bracket, current depth: " << brackets
                      << std::endl;
            if (brackets == 0) break;
            brackets--;
        } else if (brackets == 0 && (c == ',' || c == ')')) {
            std::cout << "Found delimiter at depth 0, breaking" << std::endl;
            break;
        }

        value += c;
        pos_++;

        std::cout << "Current value: '" << value << "'" << std::endl;
    }

    value = trim(value);
    std::cout << "Final parsed expression: '" << value << "'" << std::endl;
    std::cout << "Stopped at position " << pos_ << ", next char: '"
              << (pos_ < sql_.size() ? sql_[pos_] : ' ') << "'" << std::endl;

    return value;
}

}  // namespace database
