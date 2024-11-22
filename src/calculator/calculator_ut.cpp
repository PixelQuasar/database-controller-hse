#include "calculator.h"
#include <gtest/gtest.h>
#include <variant>
#include <vector>

using namespace calculator;
using namespace database;

class CalculatorTest : public ::testing::Test {
protected:
    Calculator calc;
};

TEST_F(CalculatorTest, AdditionInt) {
    EXPECT_EQ(std::get<int>(calc.add(3, 5)), 8);
}

TEST_F(CalculatorTest, AdditionDouble) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.add(3.5, 2.5)), 6.0);
}

TEST_F(CalculatorTest, SubtractionInt) {
    EXPECT_EQ(std::get<int>(calc.subtract(10, 2)), 8);
}

TEST_F(CalculatorTest, SubtractionDouble) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.subtract(10.5, 2.5)), 8.0);
}

TEST_F(CalculatorTest, MultiplicationInt) {
    EXPECT_EQ(std::get<int>(calc.multiply(4, 2)), 8);
}

TEST_F(CalculatorTest, MultiplicationDouble) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.multiply(4.5, 2.0)), 9.0);
}

TEST_F(CalculatorTest, DivisionInt) {
    EXPECT_EQ(std::get<int>(calc.divide(20, 4)), 5);
}

TEST_F(CalculatorTest, DivisionDouble) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.divide(20.0, 4.0)), 5.0);
}

TEST_F(CalculatorTest, DivisionByZero) {
    EXPECT_THROW(calc.divide(10, 0), std::invalid_argument);
}

TEST_F(CalculatorTest, LogicalAnd) {
    EXPECT_EQ(std::get<bool>(calc.evaluate("true && false")), false);
}

TEST_F(CalculatorTest, LogicalOr) {
    EXPECT_EQ(std::get<bool>(calc.evaluate("true || false")), true);
}

TEST_F(CalculatorTest, LogicalXor) {
    EXPECT_EQ(std::get<bool>(calc.evaluate("true ^^ false")), true);
}

TEST_F(CalculatorTest, StringConcatenation) {
    EXPECT_EQ(std::get<std::string>(calc.evaluate("\"Hello\" + \" World\"")), "Hello World");
}

TEST_F(CalculatorTest, StringComparison) {
    EXPECT_EQ(std::get<bool>(calc.evaluate("\"abc\" == \"abc\"")), true);
    EXPECT_EQ(std::get<bool>(calc.evaluate("\"abc\" != \"def\"")), true);
    EXPECT_EQ(std::get<bool>(calc.evaluate("\"abc\" < \"def\"")), true);
    EXPECT_EQ(std::get<bool>(calc.evaluate("\"def\" > \"abc\"")), true);
}

TEST_F(CalculatorTest, ComplexArithmetic) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("3 + 5 * 2 - 4 / 2")), 3 + 5 * 2 - 4 / 2.0);
}

TEST_F(CalculatorTest, LogicalWithArithmetic) {
    EXPECT_EQ(std::get<bool>(calc.evaluate("3 > 2 && 5 == 5")), true);
    EXPECT_EQ(std::get<bool>(calc.evaluate("10 / 2 == 5 || false")), true);
    EXPECT_EQ(std::get<bool>(calc.evaluate("10 / 2 == 4 && true")), false);
}

TEST_F(CalculatorTest, NegativeNumbers) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("-3 + 5")), 2.0);
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("-(3 + 2)")), -5.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}