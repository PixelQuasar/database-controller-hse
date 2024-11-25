#ifndef DATABASE_CONTROLLER_HSE_PARSER_H
#define DATABASE_CONTROLLER_HSE_PARSER_H

#include "../AST/SQLStatement.h"
#include <string>
#include <memory>
#include <cctype>
#include <stdexcept>

namespace database {

    class Parser {
    public:
        static std::shared_ptr<SQLStatement> parse(const std::string& sql);

    private:
        Parser(const std::string& sql);
        std::shared_ptr<SQLStatement> parseStatement();
        std::shared_ptr<CreateTableStatement> parseCreateTable();
        std::shared_ptr<InsertStatement> parseInsert();
        std::shared_ptr<SelectStatement> parseSelect();
        std::shared_ptr<UpdateStatement> parseUpdate();
        std::unordered_map<std::string, std::string> parseAssignValues();

        void skipWhitespace();
        bool matchCharacter(char expected);
        bool hasInvalidEquals(const std::string& expr);
        std::string trim(const std::string& s);
        bool isBooleanLiteral(const std::string& expr);
        std::string parseIdentifier();
        std::string parseToken();
        bool matchKeyword(const std::string& keyword);
        bool isEnd() const;

        std::string sql_;
        size_t pos_;

        std::string parseStringLiteral();
        std::string parseExpression();
    };

} // namespace database

#endif // DATABASE_CONTROLLER_HSE_PARSER_H 