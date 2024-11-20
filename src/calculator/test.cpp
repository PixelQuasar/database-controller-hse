#include "Calculator.h"
#include <gtest/gtest.h>
#include <variant>
#include <vector>

using namespace database;

class CalculatorTest : public ::testing::Test {
protected:
    Calculator calc;
};

TEST_F(CalculatorTest, Addition) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("3 + 5")), 8.0);
}

TEST_F(CalculatorTest, Subtraction) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("10 - 2")), 8.0);
}

TEST_F(CalculatorTest, Multiplication) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("4 * 2.5")), 10.0);
}

TEST_F(CalculatorTest, Division) {
    EXPECT_DOUBLE_EQ(std::get<double>(calc.evaluate("20 / 4")), 5.0);
    EXPECT_THROW(calc.evaluate("10 / 0"), std::invalid_argument);
}

TEST_F(CalculatorTest, StringConcatenation) {
    EXPECT_EQ(std::get<std::string>(calc.evaluate("\"Hello\" + \" World\"")), "Hello World");
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

TEST_F(CalculatorTest, BytesLength) {
    auto result = std::get<std::vector<uint8_t>>(calc.evaluate("ABC"));
    std::vector<uint8_t> expected = {'A', 'B', 'C'};
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(result, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
