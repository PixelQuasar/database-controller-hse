#include "Parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

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
        if (sql_[pos_] == '.') {
            pos_++;
            columnName += '.' + parseIdentifier();
        }
        skipWhitespace();

        if (pos_ >= sql_.size() || sql_[pos_] != '=') {
            throw std::runtime_error("Expected '=' after column name: " +
                                     columnName);
        }
        pos_++;
        skipWhitespace();

        std::string value = parseExpression();

        columnValuePairs[columnName] = value;
        firstColumn = false;

        if (pos_ >= sql_.size()) {
            throw std::runtime_error("Unexpected end of input after value");
        }
        if (sql_[pos_] == ',') {
            pos_++;
            skipWhitespace();
            if (pos_ >= sql_.size() ||
                (!std::isalpha(sql_[pos_]) && sql_[pos_] != '_')) {
                throw std::runtime_error("Expected column name after comma");
            }
            continue;
        }
        if (sql_[pos_] == ')') {
            pos_++;
            break;
        }
        throw std::runtime_error("Expected ',' or ')' after value");
    }
    return columnValuePairs;
}

std::shared_ptr<SQLStatement> Parser::parseStatement() {
    skipWhitespace();
    if (matchKeyword("CREATE")) {
        skipWhitespace();
        if (matchKeyword("TABLE")) {
            return parseCreateTable();
        } else if (matchKeyword("ORDERED")) {
            return parseCreateIndex(IndexType::ORDERED);
        } else if (matchKeyword("UNORDERED")) {
            return parseCreateIndex(IndexType::UNORDERED);
        } else {
            throw std::runtime_error("Expected ORDERED or UNORDERED after CREATE");
        }
    } else if (matchKeyword("INSERT")) {
        if (!matchKeyword("INTO")) {
            throw std::runtime_error("Expected INTO after INSERT");
        }
        return parseInsert();
    } else if (matchKeyword("SELECT")) {
        return parseSelect();
    } else if (matchKeyword("UPDATE")) {
        return parseUpdate();
    } else if (matchKeyword("DELETE")) {
        if (!matchKeyword("FROM")) {
            throw std::runtime_error("Expected FROM after DELETE");
        }
        return parseDelete();
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
        column.type = ColumnDefinition::stringToDataTypeName(columnType);

        while (pos_ < sql_.size()) {
            skipWhitespace();
            if (sql_[pos_] == ',' || sql_[pos_] == ')') {
                break;
            } else if (matchKeyword("UNIQUE")) {
                column.isUnique = true;
            } else if (matchKeyword("AUTOINCREMENT")) {
                if (column.type != DataTypeName::INT) {
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
    std::vector<ColumnStatement> columns = {};

    do {
        std::string rawColumn = parseIdentifier();
        std::string table, name;
        if (matchCharacter('.')) {
            table = rawColumn;
            rawColumn = parseIdentifier();
        } else {
            table = "";
        }
        columns.push_back({rawColumn, table});
    } while (sql_[pos_++] == ',');

    selectStmt->columnData = columns;

    skipWhitespace();

    if (!matchKeyword("FROM")) {
        throw std::runtime_error(
            "Expected FROM after column list in SELECT statement.");
    }

    std::string tableName = parseIdentifier();
    selectStmt->tableName = tableName;

    skipWhitespace();

    if (sql_[pos_] == ';') {
        return selectStmt;
    }

    std::string modifier = parseIdentifier();

    if (modifier == "WHERE") {
        std::string predicate;
        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            predicate += sql_[pos_++];
        }
        predicate = trim(predicate);
        selectStmt->predicate = predicate;
    } else if (modifier == "JOIN") {
        skipWhitespace();
        selectStmt->foreignTableName = parseIdentifier();

        skipWhitespace();
        if (!matchKeyword("ON")) {
            throw std::runtime_error("Expected ON after JOIN");
        }

        std::string joinPredicate;
        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            if (pos_ + 5 < sql_.size() && sql_.substr(pos_, 5) == "WHERE") {
                break;
            }
            joinPredicate += sql_[pos_++];
        }
        joinPredicate = trim(joinPredicate);

        selectStmt->joinPredicate = joinPredicate;

        skipWhitespace();

        if (matchKeyword("WHERE")) {
            std::string predicate;
            while (pos_ < sql_.size() && sql_[pos_] != ';') {
                predicate += sql_[pos_++];
            }
            predicate = trim(predicate);

            selectStmt->predicate = predicate;
        }
    }

    return selectStmt;
}

std::shared_ptr<UpdateStatement> Parser::parseUpdate() {
    auto updateStmt = std::make_unique<UpdateStatement>();
    skipWhitespace();

    std::string tableName = parseIdentifier();
    updateStmt->tableName = tableName;

    skipWhitespace();

    std::string modifier = parseIdentifier();

    if (modifier == "SET") {
        skipWhitespace();
        pos_++;
        std::unordered_map<std::string, std::string> rawValues =
            parseAssignValues();

        for (const auto& [str, value] : rawValues) {
            ColumnStatement column;

            std::size_t dotIndex = str.find('.');
            if (dotIndex != std::string::npos) {
                column.table = str.substr(0, dotIndex);
                column.name = str.substr(dotIndex + 1);
            } else {
                column.name = str;
            }

            updateStmt->newValues[column] = value;
        }
    } else if (modifier == "JOIN") {
        skipWhitespace();
        updateStmt->foreignTableName = parseIdentifier();

        skipWhitespace();
        if (!matchKeyword("ON")) {
            throw std::runtime_error("Expected ON after JOIN");
        }

        std::string joinPredicate;
        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            if (pos_ + 3 < sql_.size() && sql_.substr(pos_, 3) == "SET") {
                break;
            }
            joinPredicate += sql_[pos_++];
        }
        joinPredicate = trim(joinPredicate);

        updateStmt->joinPredicate = joinPredicate;

        skipWhitespace();

        if (matchKeyword("SET")) {
            pos_++;
            std::unordered_map<std::string, std::string> rawValues =
                parseAssignValues();

            for (const auto& [str, value] : rawValues) {
                ColumnStatement column;

                std::size_t dotIndex = str.find('.');
                if (dotIndex != std::string::npos) {
                    column.table = str.substr(0, dotIndex);
                    column.name = str.substr(dotIndex + 1);
                } else {
                    column.name = str;
                }

                updateStmt->newValues[column] = value;
            }
        }
    }

    skipWhitespace();
    if (matchKeyword("WHERE")) {
        std::string predicate;
        skipWhitespace();

        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            predicate += sql_[pos_];
            pos_++;
        }

        std::string cleanPredicate;
        bool lastWasSpace = true;

        for (char c : predicate) {
            if (std::isspace(c)) {
                if (!lastWasSpace) {
                    cleanPredicate += ' ';
                    lastWasSpace = true;
                }
            } else {
                cleanPredicate += c;
                lastWasSpace = false;
            }
        }

        if (!cleanPredicate.empty() && cleanPredicate.back() == ' ') {
            cleanPredicate.pop_back();
        }

        updateStmt->predicate = cleanPredicate;
    }

    return updateStmt;
}

std::shared_ptr<DeleteStatement> Parser::parseDelete() {
    auto deleteStmt = std::make_unique<DeleteStatement>();
    skipWhitespace();
    deleteStmt->tableName = parseIdentifier();

    skipWhitespace();

    if (matchKeyword("WHERE")) {
        std::string predicate;
        skipWhitespace();

        while (pos_ < sql_.size() && sql_[pos_] != ';') {
            predicate += sql_[pos_++];
        }

        std::string cleanPredicate;
        bool lastWasSpace = true;

        for (char c : predicate) {
            if (std::isspace(c)) {
                if (!lastWasSpace) {
                    cleanPredicate += ' ';
                    lastWasSpace = true;
                }
            } else {
                cleanPredicate += c;
                lastWasSpace = false;
            }
        }

        if (!cleanPredicate.empty() && cleanPredicate.back() == ' ') {
            cleanPredicate.pop_back();
        }

        deleteStmt->predicate = cleanPredicate;
    }

    return deleteStmt;
}

std::shared_ptr<CreateIndexStatement> Parser::parseCreateIndex(IndexType indexType) {
    CreateIndexStatement createIndexStmt;
    createIndexStmt.indexType = indexType;

    if (!matchKeyword("INDEX")) {
        throw std::runtime_error("Expected INDEX after index type");
    }

    if (!matchKeyword("ON")) {
        throw std::runtime_error("Expected ON after INDEX");
    }

    createIndexStmt.tableName = parseIdentifier();
    skipWhitespace();

    if (!matchKeyword("BY")) {
        throw std::runtime_error("Expected BY after table name in CREATE INDEX");
    }

    skipWhitespace();

    while (pos_ < sql_.size()) {
        std::string column = parseIdentifier();
        createIndexStmt.columns.push_back(column);
        skipWhitespace();

        if (pos_ < sql_.size() && sql_[pos_] == ',') {
            pos_++;
            skipWhitespace();
            continue;
        } else if (pos_ < sql_.size() && sql_[pos_] == ';') {
            pos_++;
            break;
        } else {
            throw std::runtime_error("Expected ',' or ';' in column list of CREATE INDEX");
        }
    }

    return std::make_shared<CreateIndexStatement>(createIndexStmt);
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

    skipWhitespace();
    if (sql_[pos_] == ',' || sql_[pos_] == ')') {
        return "";
    }

    bool expectingMore = false;
    while (pos_ < sql_.size()) {
        char c = sql_[pos_];

        if (c == '"') {
            value += parseStringLiteral();
            expectingMore = false;
            continue;
        }

        if (c == '(') {
            brackets++;
        } else if (c == ')') {
            if (brackets == 0) {
                break;
            }
            brackets--;
        } else if (brackets == 0 && c == ',' && !expectingMore) {
            break;
        } else if (brackets == 0 && c == ')' && !expectingMore) {
            break;
        }

        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' ||
            c == '|') {
            expectingMore = true;
        } else if (!std::isspace(c)) {
            expectingMore = false;
        }

        value += c;
        pos_++;
    }
    value = trim(value);
    return value;
}

}  // namespace database
