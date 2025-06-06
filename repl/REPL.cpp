#include "REPL.hpp"


const std::regex REPL::s_argsPattern("(?:([a-zA-Z_]+)=([\\-0-9.]+)*)");

REPL::REPL(polishd::Grammar grammar) : m_compiler(grammar)
{
    m_commands["save"] = [&](){save();};
    m_commands["eval"] = [&](){ eval();};
    m_commands["evals"] = [&](){ evalSaved();};
    m_commands["show"] = [&](){show();};
    m_commands["list"] = [&](){listSaved();};
    m_commands["delete"] = [&](){deleteSaved();};
    m_commands["clear"] = [&](){clear();};
    m_commands["grammar"] = [&](){showGrammar();};
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
    m_functions.insert_or_assign(name, m_compiler.compile(expression));
}

void REPL::eval()
{
    std::string tail;
    std::getline(std::cin >> std::ws, tail);
    auto iterator = std::sregex_iterator(tail.begin(),
                                         tail.end(),
                                         s_argsPattern);
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
    polishd::Function f = m_compiler.compile(expression);
    double result = f(args);
    std::cout << result << std::endl;
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
                                         s_argsPattern);
    auto end = std::sregex_iterator();
    for (; iterator != end; ++iterator)
        args.insert_or_assign((*iterator)[1].str(), std::stod((*iterator)[2].str()));
    auto lookup = m_functions.find(name);
    if (lookup != m_functions.end())
        std::cout << lookup->second(args) << std::endl;
    else
        std::cout << "No function with name '" << name << '\'' << std::endl;
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

void REPL::listSaved()
{
    if(!m_functions.empty())
    {
        for (const auto& pair: m_functions)
            std::cout << pair.first << ": " << pair.second.infix();
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

void REPL::showGrammar()
{
    const polishd::Grammar& grammar = m_compiler.grammar();
    std::cout << "Constants: ";
    for (const auto& pair: grammar.constants())
        std::cout << pair.first << '=' << pair.second << ", ";
    std::cout << std::endl << "Prefix Functions: ";
    for (const auto& pair: grammar.prefix())
        std::cout << pair.first << ", ";
    std::cout << std::endl << "Binary Operations (precedence): ";
    for (const auto& pair: grammar.binary())
        std::cout << pair.first << '(' << (short) pair.second.precedence << "), ";
    std::cout << std::endl << "Postfix Functions: ";
    for (const auto& pair: grammar.postfix())
        std::cout << pair.first << ", ";
    std::cout << std::endl;
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
            "# > list        - all saved functions                           #\n"
            "# > delete name - delete the 'name' function                    #\n"
            "# > clear       - delete all functions                          #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# > grammar    - all available operators and constants          #\n"
            "# > help       - general app usage guide                        #\n"
            "# > exit       - terminate the program                          #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# ARGUMENTS (args...):                                          #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# 'args...' is a sequence of 'name=value' pairs, where:         #\n"
            "# > name - argument signature to seek in function for           #\n"
            "# > value - int or float number to substitute 'name' with       #\n"
            "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
            "# Pairs should be separated with at least one whitespace        #\n"
            "# Order of pairs does not matter                                #\n"
            "# Extra pairs are ignored                                       #\n"
            "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
    );
    std::cout << message;
}
