#define _USE_MATH_DEFINES
#include <math.h>

#include "REPL.hpp"

static void setupDemoGrammar(polishd::Grammar& grammar)
{
    // constants
    grammar.addConstant("pi", M_PI);
    grammar.addConstant("e", M_E);

    // prefix operators
    grammar.addPrefixOperator("-", [](double x) -> double { return -x; });
    grammar.addPrefixOperator("exp", std::exp);
    grammar.addPrefixOperator("sin", std::sin);
    grammar.addPrefixOperator("cos", std::cos);
    grammar.addPrefixOperator("floor", std::floor);
    grammar.addPrefixOperator("ceil", std::ceil);
    grammar.addPrefixOperator("round", std::round);
    grammar.addPrefixOperator("abs", std::abs);
    
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
