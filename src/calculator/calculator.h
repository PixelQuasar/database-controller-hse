#ifndef DATABASE_CONTROLLER_HSE_CALCULATOR_H
#define DATABASE_CONTROLLER_HSE_CALCULATOR_H

#include <string>
#include <vector>
#include <variant>
#include "../types.h"

namespace calculator {

    using Value = database::DBType;

    class Calculator {
    public:
        Value add(const Value& a, const Value& b);
        Value subtract(const Value& a, const Value& b);
        Value multiply(const Value& a, const Value& b);
        Value divide(const Value& a, const Value& b);

        Value evaluate(const std::string& expression);
    
    private:
        std::vector<std::string> tokenize(const std::string& expression);
        int getPrecedence(const std::string& operator_);
        bool isOperator(const std::string& token);
        Value applyOperator(const std::string& op, const Value& a, const Value& b);
        bool applyLogicalOperator(const std::string& op, bool a, bool b);
    };

} // calculator

#endif //DATABASE_CONTROLLER_HSE_CALCULATOR_H