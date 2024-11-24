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
        static std::unique_ptr<SQLStatement> parse(const std::string& sql);

    private:
        Parser(const std::string& sql);
        std::unique_ptr<SQLStatement> parseStatement();
        std::unique_ptr<CreateTableStatement> parseCreateTable();
        std::unique_ptr<InsertStatement> parseInsert();
        std::unique_ptr<SelectStatement> parseSelect();

        void skipWhitespace();
        std::string parseIdentifier();
        bool matchKeyword(const std::string& keyword);
        bool isEnd() const;

        std::string sql_;
        size_t pos_;
    };

} // namespace database

#endif // DATABASE_CONTROLLER_HSE_PARSER_H 