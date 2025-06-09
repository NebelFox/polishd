#define _USE_MATH_DEFINES
#include <math.h>

#include "REPL.hpp"

static void setupDemoGrammar(polishd::Grammar& grammar)
{
    // constants
    grammar.addConstant("pi", M_PI);
    grammar.addConstant("e", M_E);

    // prefix operators
    #define UNARY(EXPR_ON_X) [](double x) -> double { return EXPR_ON_X; }
    grammar.addPrefixOperator("-", UNARY(-x));
    grammar.addPrefixOperator("exp", UNARY(std::exp(x)));
    grammar.addPrefixOperator("sin", UNARY(std::sin(x)));
    grammar.addPrefixOperator("cos", UNARY(std::cos(x)));
    grammar.addPrefixOperator("floor", UNARY(std::floor(x)));
    grammar.addPrefixOperator("ceil", UNARY(std::ceil(x)));
    grammar.addPrefixOperator("round", UNARY(std::round(x)));
    grammar.addPrefixOperator("abs", UNARY(std::abs(x)));
    #undef UNARY
    
    // binary operators
    #define BINARY(EXPR_ON_A_AND_B) [](double a, double b) -> double { return EXPR_ON_A_AND_B; }
    grammar.addBinaryOperator("+", BINARY(a+b), 1);
    grammar.addBinaryOperator("-", BINARY(a-b), 1);
    grammar.addBinaryOperator("*", BINARY(a*b), 2);
    grammar.addBinaryOperator("/", BINARY(a/b), 2);
    grammar.addBinaryOperator("^", pow, 3);
    #undef BINARY

    // postfix operators
    grammar.addPostfixOperator("!", [](double x) -> double
    {
        auto n = (size_t) x;
        size_t result = 1;
        while (n > 1)
        {
            result *= n--;
        }
        return (double) result;
    });
}


int main()
{
    polishd::Grammar grammar;
    setupDemoGrammar(grammar);
    REPL(grammar).start();
}
