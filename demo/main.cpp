#define _USE_MATH_DEFINES
#include <math.h>

#include "REPL.hpp"

static void setup_demo_grammar(polishd::Grammar& grammar)
{
    // constants
    grammar.add_constant("pi", M_PI);
    grammar.add_constant("e", M_E);

    // prefix operators
    grammar.add_prefix_operator("-", [](double x) -> double { return -x; });
    grammar.add_prefix_operator("exp", std::exp);
    grammar.add_prefix_operator("sin", std::sin);
    grammar.add_prefix_operator("cos", std::cos);
    grammar.add_prefix_operator("floor", std::floor);
    grammar.add_prefix_operator("ceil", std::ceil);
    grammar.add_prefix_operator("round", std::round);
    grammar.add_prefix_operator("abs", std::abs);
    
    // binary operators
    #define BINARY(EXPR_ON_A_AND_B) [](double a, double b) -> double { return EXPR_ON_A_AND_B; }
    grammar.add_binary_operator("+", BINARY(a+b), 1);
    grammar.add_binary_operator("-", BINARY(a-b), 1);
    grammar.add_binary_operator("*", BINARY(a*b), 2);
    grammar.add_binary_operator("/", BINARY(a/b), 2);
    grammar.add_binary_operator("^", pow, 3);
    #undef BINARY

    // postfix operators
    grammar.add_postfix_operator("!", [](double x) -> double
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
    setup_demo_grammar(grammar);
    REPL(grammar).start();
}
