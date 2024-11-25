#include <gtest/gtest.h>

#include <variant>
#include <vector>

#include "Calculator.h"

using namespace calculator;
using namespace database;

class CalculatorTest : public ::testing::Test {
   protected:
    Calculator calc;
};

TEST_F(CalculatorTest, AdditionInt) {
    EXPECT_EQ(safeGet<int>(calc.add(3, 5)), 8);
}

TEST_F(CalculatorTest, AdditionDouble) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.add(3.5, 2.5)), 6.0);
    EXPECT_EQ(safeGet<int>(calc.add(3.5, 2.5)), 6);
}

TEST_F(CalculatorTest, SubtractionInt) {
    EXPECT_EQ(safeGet<int>(calc.subtract(10, 2)), 8);
}

TEST_F(CalculatorTest, SubtractionDouble) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.subtract(10.5, 2.5)), 8.0);
}

TEST_F(CalculatorTest, MultiplicationInt) {
    EXPECT_EQ(safeGet<int>(calc.multiply(4, 2)), 8);
}

TEST_F(CalculatorTest, MultiplicationDouble) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.multiply(4.5, 2.0)), 9.0);
}

TEST_F(CalculatorTest, DivisionInt) {
    EXPECT_EQ(safeGet<int>(calc.divide(20, 4)), 5);
}

TEST_F(CalculatorTest, DivisionDouble) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.divide(20.0, 4.0)), 5.0);
}

TEST_F(CalculatorTest, DivisionByZero) {
    EXPECT_THROW(calc.divide(10, 0), std::invalid_argument);
}

TEST_F(CalculatorTest, LogicalAnd) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("true && false")), false);
}

TEST_F(CalculatorTest, LogicalOr) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("true || false")), true);
}

TEST_F(CalculatorTest, LogicalXor) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("true ^^ false")), true);
}

TEST_F(CalculatorTest, StringConcatenation) {
    EXPECT_EQ(safeGet<std::string>(calc.evaluate("\"Hello\" + \" World\"")),
              "Hello World");
}

TEST_F(CalculatorTest, StringComparison) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("\"abc\" == \"abc\"")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("\"abc\" != \"def\"")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("\"abc\" < \"def\"")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("\"def\" > \"abc\"")), true);
}

TEST_F(CalculatorTest, ComplexArithmetic) {
    EXPECT_EQ(safeGet<int>(calc.evaluate("3 + 5 * 2 - 4 / 2")), 11);
}

TEST_F(CalculatorTest, LogicalWithArithmetic) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("3 > 2 && 5 == 5")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("10 / 2 == 5 || false")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("10 / 2 == 4 && true")), false);
}

TEST_F(CalculatorTest, NegativeNumbers) {
    EXPECT_EQ(safeGet<int>(calc.evaluate("-3 + 5")), 2);
    EXPECT_EQ(safeGet<int>(calc.evaluate("-(3 + 2)")), -5);
}

TEST_F(CalculatorTest, ComplexExpressionWithParentheses) {
    EXPECT_DOUBLE_EQ(
        safeGet<double>(calc.evaluate("3.5 + (2 * (1 + 3)) - 4 / 2")),
        3.5 + (2 * (1 + 3)) - 4 / 2);
}

TEST_F(CalculatorTest, NestedParentheses) {
    EXPECT_DOUBLE_EQ(safeGet<int>(calc.evaluate("((2 + 3) * (4 - 1)) / 2")),
                     ((2 + 3) * (4 - 1)) / 2);
}

TEST_F(CalculatorTest, MixedIntegerAndDouble) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.evaluate("5 + 3.2 * 2 - 1.5 / 0.5")),
                     5 + 3.2 * 2 - 1.5 / 0.5);
}

TEST_F(CalculatorTest, ComplexLogicalAndArithmetic) {
    EXPECT_EQ(safeGet<bool>(calc.evaluate("(3 > 2) && ((5 + 2) == 7)")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("((10 / 2) == 5) || (3 < 1)")), true);
    EXPECT_EQ(safeGet<bool>(calc.evaluate("((10 / 2) == 4) && (2 > 1)")),
              false);
}

TEST_F(CalculatorTest, DeeplyNestedParentheses) {
    EXPECT_DOUBLE_EQ(safeGet<double>(calc.evaluate(
                         "((((1 + 2) * 3) - 4) / 2) + (5 * (6 - (7 + 8)))")),
                     ((((1 + 2) * 3) - 4) / 2) + (5 * (6 - (7 + 8))));
}

TEST_F(CalculatorTest, ExternalValues) {
    std::unordered_map<std::string, std::string> values = {{"a", "5"},
                                                           {"b", "3"}};
    EXPECT_EQ(safeGet<int>(calc.evaluate("a + b", values)), 8);
    EXPECT_EQ(safeGet<int>(calc.evaluate("a * b", values)), 15);
    EXPECT_EQ(safeGet<int>(calc.evaluate("a - b", values)), 2);
    EXPECT_EQ(safeGet<int>(calc.evaluate("a / b", values)), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}