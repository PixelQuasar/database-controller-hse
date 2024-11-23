#include "Calculator.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <variant>

namespace calculator {

    using Value = database::DBType;

    Value Calculator::add(const Value& a, const Value& b) {
        return applyOperator("+", a, b);
    }

    Value Calculator::subtract(const Value& a, const Value& b) {
        return applyOperator("-", a, b);
    }

    Value Calculator::multiply(const Value& a, const Value& b) {
        return applyOperator("*", a, b);
    }

    Value Calculator::divide(const Value& a, const Value& b) {
        return applyOperator("/", a, b);
    }

    Value Calculator::evaluate(const std::string& expression) {
        std::vector<std::string> tokens = tokenize(expression);
        std::vector<std::string> outputQueue;
        std::stack<std::string> operatorStack;

        for (const auto& token : tokens) {
            if (std::isdigit(token[0]) || 
                (token[0] == '-' && token.size() > 1 && std::isdigit(token[1])) ||
                (token[0] == '.' && token.size() > 1)) {
                outputQueue.push_back(token);
            }
            else if (token == "true" || token == "false") {
                outputQueue.push_back(token);
            }
            else if (isOperator(token)) {
                while (!operatorStack.empty() && isOperator(operatorStack.top()) &&
                       getPrecedence(operatorStack.top()) >= getPrecedence(token)) {
                    outputQueue.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            }
            else if (token == "(") {
                operatorStack.push(token);
            }
            else if (token == ")") {
                while (!operatorStack.empty() && operatorStack.top() != "(") {
                    outputQueue.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                if (operatorStack.empty()) {
                    throw std::invalid_argument("Incorrect brackets.");
                }
                operatorStack.pop();
            }
            else {
                outputQueue.push_back(token);
            }
        }

        while (!operatorStack.empty()) {
            if (operatorStack.top() == "(" || operatorStack.top() == ")") {
                throw std::invalid_argument("Incorrect brackets.");
            }
            outputQueue.push_back(operatorStack.top());
            operatorStack.pop();
        }

        std::stack<Value> valueStack;
        for (const auto& token : outputQueue) {
            if (isOperator(token)) {
                if (token == "-" && valueStack.size() == 1) {
                    Value a = valueStack.top(); valueStack.pop();
                    Value result = applyOperator("negate", a, 0);
                    valueStack.push(result);
                } else {
                    if (valueStack.size() < 2) {
                        throw std::invalid_argument("Not enough operands for the operation.");
                    }
                    Value b = valueStack.top(); valueStack.pop();
                    Value a = valueStack.top(); valueStack.pop();
                    Value result = applyOperator(token, a, b);
                    valueStack.push(result);
                }
            }
            else {
                if (std::isdigit(token[0]) || 
                    (token[0] == '-' && token.size() > 1 && std::isdigit(token[1])) ||
                    (token[0] == '.' && token.size() > 1)) {
                    if (token.find('.') != std::string::npos) {
                        valueStack.push(std::stod(token));
                    }
                    else {
                        valueStack.push(std::stoi(token));
                    }
                }
                else if (token == "true" || token == "false") {
                    valueStack.push(token == "true");
                }
                else {
                    valueStack.emplace(std::string(token));
                }
            }
        }

        if (valueStack.size() != 1) {
            throw std::invalid_argument("Incorrect expression.");
        }

        return valueStack.top();
    }

    std::vector<std::string> Calculator::tokenize(const std::string& expression) {
        std::vector<std::string> tokens;
        std::string token;
        bool inString = false;

        for (size_t i = 0; i < expression.length(); ++i) {
            char ch = expression[i];

            if (inString) {
                if (ch == '"') {
                    inString = false;
                    tokens.push_back(token);
                    token.clear();
                } else {
                    token += ch;
                }
                continue;
            }

            if (std::isspace(ch)) {
                continue;
            }

            if (ch == '"') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                inString = true;
                continue;
            }

            if (std::isdigit(ch) || ch == '.' || (ch == '-' && (i == 0 || expression[i-1] == '(' || isOperator(std::string(1, expression[i-1]))))) {
                token += ch;
            }
            else if (ch == 't' || ch == 'f') {
                size_t start = i;
                while (i < expression.length() && std::isalpha(expression[i])) {
                    token += expression[i];
                    ++i;
                }
                --i;
                tokens.push_back(token);
                token.clear();
            }
            else {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                if (ch == '(' || ch == ')') {
                    tokens.emplace_back(1, ch);
                }
                else {
                    std::string op;
                    op += ch;
                    if ((ch == '&' || ch == '|' || ch == '=' || ch == '!') && (i + 1 < expression.length())) {
                        if (expression[i + 1] == ch || expression[i + 1] == '=') {
                            op += expression[i + 1];
                            ++i;
                        }
                    }
                    else if (ch == '^' && (i + 1 < expression.length()) && expression[i + 1] == '^') {
                        op += expression[i + 1];
                        ++i;
                    }
                    tokens.push_back(op);
                }
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }

    int Calculator::getPrecedence(const std::string& operator_) {
        if (operator_ == "||") {
            return 1;
        }
        if (operator_ == "&&") {
            return 2;
        }
        if (operator_ == "==" || operator_ == "!=") {
            return 3;
        }
        if (operator_ == "<" || operator_ == "<=" || operator_ == ">" || operator_ == ">=") {
            return 4;
        }
        if (operator_ == "+" || operator_ == "-") {
            return 5;
        }
        if (operator_ == "*" || operator_ == "/" || operator_ == "%") {
            return 6;
        }
        return 0;
    }

    bool Calculator::isOperator(const std::string& token) {
        static const std::vector<std::string> operators = {
            "+", "-", "*", "/", "%", "&&", "||", "!", "^^",
            "<", ">", "<=", ">=", "==", "!="
        };
        return std::find(operators.begin(), operators.end(), token) != operators.end();
    }

    Value Calculator::applyOperator(const std::string& op, const Value& a, const Value& b) {
        return std::visit([&](auto&& lhs, auto&& rhs) -> Value {
            using T1 = std::decay_t<decltype(lhs)>;
            using T2 = std::decay_t<decltype(rhs)>;

            if (op == "negate") {
                if constexpr (std::is_same_v<T1, int>) {
                    return -lhs;
                }
                if constexpr (std::is_same_v<T1, double>) {
                    return -lhs;
                }
            }

            if constexpr ((std::is_same_v<T1, int> || std::is_same_v<T1, double>) &&
                          (std::is_same_v<T2, int> || std::is_same_v<T2, double>)) {
                if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) {
                    if (op == "+") return lhs + rhs;
                    if (op == "-") return lhs - rhs;
                    if (op == "*") return lhs * rhs;
                    if (op == "/") {
                        if (rhs == 0) {
                            throw std::invalid_argument("Division by zero is not possible!");
                        }
                        return lhs / rhs;
                    }
                    if (op == "%") {
                        if (rhs == 0) {
                            throw std::invalid_argument("Division by zero is not possible!");
                        }
                        return lhs % rhs;
                    }
                }
                else {
                    double lhs_val = lhs;
                    double rhs_val = rhs;
                    if (op == "+") return lhs_val + rhs_val;
                    if (op == "-") return lhs_val - rhs_val;
                    if (op == "*") return lhs_val * rhs_val;
                    if (op == "/") {
                        if (rhs_val == 0.0) {
                            throw std::invalid_argument("Division by zero is not possible!");
                        }
                        return lhs_val / rhs_val;
                    }
                }
            }

            if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) {
                if (op == "&&") return lhs && rhs;
                if (op == "||") return lhs || rhs;
                if (op == "^^") return lhs != rhs;
            }

            if constexpr (std::is_same_v<T1, std::string> && std::is_same_v<T2, std::string>) {
                if (op == "+") return lhs + rhs;
            }

            if constexpr (std::is_same_v<T1, T2> &&
                          (std::is_same_v<T1, int> || std::is_same_v<T1, double> || std::is_same_v<T1, std::string>)) {
                if (op == "==") return lhs == rhs;
                if (op == "!=") return lhs != rhs;
                if (op == "<") return lhs < rhs;
                if (op == "<=") return lhs <= rhs;
                if (op == ">") return lhs > rhs;
                if (op == ">=") return lhs >= rhs;
            }

            throw std::invalid_argument("Incorrect operation: " + op);
        }, a, b);
    }

} // calculator