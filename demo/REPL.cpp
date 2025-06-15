#include "REPL.hpp"

#include <iostream>
#include <functional>

const std::regex REPL::s_args_pattern("(?:([a-zA-Z_]+)=([\\-0-9.]+)*)");

REPL::REPL(polishd::Grammar& grammar) : m_grammar(grammar)
{
    #define BIND(METHOD) std::bind(&REPL::METHOD, this);
    m_commands["save"] = BIND(save);
    m_commands["eval"] = BIND(eval);
    m_commands["evals"] = BIND(eval_saved);
    m_commands["show"] = BIND(show);
    m_commands["list"] = BIND(list_saved);
    m_commands["delete"] = BIND(delete_saved);
    m_commands["clear"] = BIND(clear);
    m_commands["grammar"] = BIND(show_grammar);
    m_commands["help"] = help;
    #undef BIND
}

void REPL::start()
{
    bool running = true;
    std::string keyword;
    help();
    std::cout << std::endl;
    while (running)
    {
        try
        {
            std::cout << "$ ";
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
        catch(const std::exception& e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}

void REPL::save()
{
    std::string name;
    std::cin >> name >> std::ws;
    std::string expression;
    std::getline(std::cin, expression);
    m_functions.insert_or_assign(name, polishd::compile(m_grammar, expression));
}

void REPL::eval()
{
    std::string tail;
    std::getline(std::cin >> std::ws, tail);
    auto [start, args] = parse_args(tail);
    polishd::Function f = polishd::compile(m_grammar, tail.substr(0, start));
    double result = f(args);
    std::cout << result << std::endl;
}

void REPL::eval_saved()
{
    std::string name;
    std::cin >> name >> std::ws;
    std::string tail;
    std::getline(std::cin, tail);
    auto lookup = m_functions.find(name);
    if(lookup == m_functions.end())
    {
        std::cout << "No function with name '" << name << '\'' << std::endl;
        return;
    }
    polishd::Args args = parse_args(tail).second;
    double result = lookup->second(args);
    std::cout << result << std::endl;
}

void REPL::show()
{
    std::string name;
    std::cin >> name;
    auto lookup = m_functions.find(name);
    if (lookup != m_functions.end())
    {
        std::cout << "Infix form: " << lookup->second.infix() << std::endl
                  << "Postfix form: " << lookup->second.postfix() << std::endl;
    }
}

void REPL::list_saved()
{
    if(!m_functions.empty())
    {
        for (const auto& pair: m_functions)
            std::cout << pair.first << ": " << pair.second.infix() << std::endl;
    }
}

void REPL::delete_saved()
{
    std::string name;
    std::cin >> name;
    m_functions.erase(name);
}

void REPL::clear()
{
    m_functions.clear();
}

void REPL::show_grammar()
{
    std::cout << "Constants: ";
    for (const auto& pair: m_grammar.constants())
        std::cout << pair.first << '=' << pair.second << ", ";
    std::cout << std::endl << "Prefix Functions: ";
    for (const auto& pair: m_grammar.prefix())
        std::cout << pair.first << ", ";
    std::cout << std::endl << "Binary Operations (precedence): ";
    for (const auto& pair: m_grammar.binary())
        std::cout << pair.first << '(' << (short) pair.second.precedence << "), ";
    std::cout << std::endl << "Postfix Functions: ";
    for (const auto& pair: m_grammar.postfix())
        std::cout << pair.first << ", ";
    std::cout << std::endl;
}

void REPL::help()
{
    static const std::string message(
        "This is a demo of `polishd` library.\n"
        "It uses postfix notation to evaluate arithmetic expressions.\n"
        "\n"
        "Commands:\n"
        "\teval EXPR ARGS...   evaluate the expression with given arguments.\n"
        "\tsave NAME EXPR      save the expression as function NAME.\n"
        "\tevals NAME ARGS...  evaluate the saved function NAME with given arguments.\n"
        "\tshow NAME           show saved function NAME in infix and postfix notation.\n"
        "\tlist                show all saved functions.\n"
        "\tdelete NAME         delete the saved function NAME.\n"
        "\tclear               delete all saved functions.\n"
        "\tgrammar             show all supported operators and constants.\n"
        "\thelp                show this message.\n"
        "\texit                stop the REPL and exit.\n"
        "\n"
        "Expressions (EXPR):\n"
        "\tA regular arithmetic expression that is valid for supported grammar (see `grammar` command).\n"
        "\tUse parentheses for subexpressions like `2*(1+2)`.\n"
        "\tPrefix and postfix functions are unary, i.e. take only one parameter,\n"
        "\tSo they could be used with parenthesis like `1 + cos 0`;\n"
        "\tAn argument wrapped in parentheses is treated as a regular subexpression, e.g. sin(pi*0.5).\n"
        "\tAn expression is evaluated from left to right, so `cos 0+1` is equivalent to `(cos 0) + 1`.\n"
        "Arguments (ARGS...):\n"
        "\tA sequence of `name=value` pairs, separated by at least one space.\n"
        "\tThe `name` must consist only of english letters and underscore (`_`).\n"
        "\tThe `value` could be either an integer, e.g. `42` or `-1`, or a float, e.g. `4.2` or `-0.3`.\n"
        "\tThe arguments could be specified in any order.\n"
        "\tArguments that are not used in the expression are ignored and won't cause an error.\n"
        "Function names (NAME):\n"
        "\tAny non-zero-length sequence of non-whitespace characters.\n"
    );
    std::cout << message;
}

std::pair<size_t, polishd::Args> REPL::parse_args(const std::string& s)
{
    polishd::Args args;
    auto iterator = std::sregex_iterator(s.begin(), s.end(), s_args_pattern);
    auto end = std::sregex_iterator();
    size_t start = std::string::npos;
    if(iterator != end)
        start = (*iterator).position(0);
    for(; iterator != end; ++iterator)
        args.insert_or_assign((*iterator)[1].str(), std::stod((*iterator)[2].str()));
    return std::make_pair(start, args);
}
