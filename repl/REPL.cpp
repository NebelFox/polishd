#define _USE_MATH_DEFINES
#include <math.h>

#include "REPL.hpp"


void REPL::setupGrammar(polishd::Grammar& grammar)
{
    grammar.addConstant("pi", M_PI);
    grammar.addConstant("e", M_E);

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


REPL::REPL() : m_argsPattern("(?:([a-zA-Z_]+)=([\\-0-9.]+)*)")
{
    setupGrammar(m_grammar);
    m_compiler = new polishd::Compiler(m_grammar);
    m_commands["save"] = [&](){save();};
    m_commands["eval"] = [&](){ eval();};
    m_commands["evals"] = [&](){ evalSaved();};
    m_commands["show"] = [&](){show();};
    m_commands["list-saved"] = [&](){listSaved();};
    m_commands["delete"] = [&](){deleteSaved();};
    m_commands["clear"] = [&](){clear();};
    m_commands["grammar"] = [&](){grammar();};
    m_commands["args-info"] = argsInfo;
    m_commands["help"] = help;

}

void REPL::start()
{
    bool running = true;
    std::string keyword;
    help();
    std::cout << std::endl;
    while (running)
    {
        std::cout << "$~";
        std::cin >> keyword;
        auto lookup = m_commands.find(keyword);
        if(lookup != m_commands.end())
            lookup->second();
        else if(keyword == "exit")
            running = false;
        else
            std::cout << "Unknown command" << std::endl;
        std::cout << std::endl;
    }
}

void REPL::save()
{
    std::string name;
    std::cin >> name >> std::ws;
    std::string expression;
    std::getline(std::cin, expression);
//        cout << "save | name=[" << name << "] expression=[" << expression << "]\n";
    m_functions.insert_or_assign(name, m_compiler->compile(expression));
}

void REPL::eval()
{
    std::string tail;
    std::getline(std::cin >> std::ws, tail);
    auto iterator = std::sregex_iterator(tail.begin(),
                                         tail.end(),
                                         m_argsPattern);
    auto end = std::sregex_iterator();
    size_t argsBegin = std::string::npos;
    if(iterator != end)
        argsBegin = (*iterator).position(0);
    polishd::Function::Args args;
    for(; iterator != end; ++iterator)
    {
        args[(*iterator)[1].str()] = std::stod((*iterator)[2].str());
    }
    std::string expression = tail.substr(0, argsBegin);
//        cout << "Expression: [" << expression << "]\n";
    polishd::Function f = m_compiler->compile(expression);
    double result = f(args);
    std::cout << "The result = " << result << std::endl;
}

void REPL::evalSaved()
{
    std::string name;
    std::cin >> name >> std::ws;
    polishd::Function::Args args;
    std::string tail;
    std::getline(std::cin, tail);
    auto iterator = std::sregex_iterator(tail.begin(),
                                         tail.end(),
                                         m_argsPattern);
    auto end = std::sregex_iterator();
    for (; iterator != end; ++iterator)
        args.insert_or_assign((*iterator)[1].str(), std::stod((*iterator)[2].str()));
    auto lookup = m_functions.find(name);
    if (lookup != m_functions.end())
        std::cout << "The result is: " << lookup->second(args) << std::endl;
    else
        std::cout << "Unknown function: '" << name << "'\n";
}

void REPL::show()
{
    std::string name;
    std::cin >> name;
    auto lookup = m_functions.find(name);
    if (lookup != m_functions.end())
    {
        std::cout << "Infix form: "
                  << lookup->second.infix()
                  << "\nPostfix form: "
                  << lookup->second.postfix()
                  << std::endl;
    }
}

void REPL::listSaved()
{
    if (m_functions.empty())
    {
        std::cout << "No functions\n";
    }
    else
    {
        for (const auto& pair: m_functions)
            std::cout << pair.first << ": " << pair.second.infix() << std::endl;
    }
}

void REPL::deleteSaved()
{
    std::string name;
    std::cin >> name;
    m_functions.erase(name);
}

void REPL::clear()
{
    m_functions.clear();
}

void REPL::grammar()
{
    std::cout << "Constants: ";
    for (const auto& pair: m_grammar.constants())
        std::cout << pair.first << '=' << pair.second << ", ";
    std::cout << "\nPrefix Functions: ";
    for (const auto& pair: m_grammar.prefix())
        std::cout << pair.first << ", ";
    std::cout << "\nBinary Operations (precedence): ";
    for (const auto& pair: m_grammar.binary())
        std::cout << pair.first << '(' << (short) pair.second.precedence << "), ";
    std::cout << "\nPostfix Functions: ";
    for (const auto& pair: m_grammar.postfix())
        std::cout << pair.first << ", ";
    std::cout << std::endl;
}

void REPL::argsInfo()
{
    static const std::string message(
            "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
            "# 'args...' is a sequence of 'name=value' pairs, where:   #\n"
            "# > name - argument signature to seek in function for     #\n"
            "# > value - int or float number to substitute 'name' with #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# Pairs should be separated with at least one whitespace  #\n"
            "# Order of pairs does not matter                          #\n"
            "# Extra pairs are ignored                                 #\n"
            "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
    );
    std::cout << message;
}

void REPL::help()
{
    static const std::string message(
            "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
            "# AVAILABLE COMMANDS:                                           #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# > eval function args...   - eval expression with given args   #\n"
            "# > save name function      - save the function as 'name'       #\n"
            "# > evals name args...      - eval saved function 'name'        #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# > show name   - 'name' function in infix & postfix notations  #\n"
            "# > list-saved  - all saved functions                           #\n"
            "# > delete name - delete the 'name' function                    #\n"
            "# > clear       - delete all functions                          #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# > grammar    - all available operators and constants          #\n"
            "# > args-info  - guide on args                                  #\n"
            "# > help       - general app usage guide                        #\n"
            "# > exit       - terminate the program                          #\n"
            "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
    );
    std::cout << message;
}

REPL::~REPL()
{
    delete m_compiler;
}

