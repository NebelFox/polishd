#define _USE_MATH_DEFINES
#include <math.h>

#include "REPL.hpp"


static void setupDemoGrammar(polishd::Grammar& grammar)
{
    // constants
    grammar.addConstant("pi", M_PI);
    grammar.addConstant("e", M_E);

    // prefix operators
    grammar.addPrefixOperator("-",
                                [](const double x) -> double
                                { return -x; });
    grammar.addPrefixOperator("exp",
                                [](const double x) -> double
                                { return std::exp(x); });
    grammar.addPrefixOperator("sin",
                                [](const double x) -> double
                                { return std::sin(x); });
    grammar.addPrefixOperator("cos",
                                [](const double x) -> double
                                { return std::cos(x); });
    grammar.addPrefixOperator("floor",
                                [](const double x) -> double
                                { return std::floor(x); });
    grammar.addPrefixOperator("ceil",
                                [](const double x) -> double
                                { return std::ceil(x); });
    grammar.addPrefixOperator("round",
                                [](const double x) -> double
                                { return std::round(x); });
    
    // binary operators
    grammar.addBinaryOperator("+",
                                [](const double a, const double b) -> double
                                { return a + b; }, 1);
    grammar.addBinaryOperator("-",
                                [](const double a, const double b) -> double
                                { return a - b; }, 1);
    grammar.addBinaryOperator("*",
                                [](const double a, const double b) -> double
                                { return a * b; }, 2);
    grammar.addBinaryOperator("/",
                                [](const double a, const double b) -> double
                                { return a / b; }, 2);
    grammar.addBinaryOperator("^", pow, 3);

    // postfix operators
    grammar.addPostfixOperator("!", [](const double x) -> double
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
