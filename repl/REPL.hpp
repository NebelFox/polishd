#ifndef INC_POLISHD_CALCULATOR_HPP
#define INC_POLISHD_CALCULATOR_HPP

#include <unordered_map>
#include <regex>
#include <functional>

#include "Grammar.hpp"
#include "Compiler.hpp"
#include "Function.hpp"


class REPL
{
public:
    REPL(polishd::Grammar& grammar);

    void start();

    void eval();
    void save();
    void show();
    void evalSaved();
    void listSaved();
    void deleteSaved();
    void clear();

    void showGrammar();
    static void help();

private:
    polishd::Compiler m_compiler;
    std::unordered_map<std::string, polishd::Function> m_functions;
    std::unordered_map<std::string, std::function<void()>> m_commands;
    static const std::regex s_argsPattern;
};


#endif //INC_POLISHD_CALCULATOR_HPP
