#ifndef INC_POLISHD_CALCULATOR_HPP
#define INC_POLISHD_CALCULATOR_HPP

#include <iostream>
#include <regex>
#include <functional>

#include "Grammar.hpp"
#include "Compiler.hpp"
#include "Function.hpp"


class REPL
{
public:
    REPL(polishd::Grammar grammar);

    void start();

    void eval();
    void save();
    void show();
    void evalSaved();
    void listSaved();
    void deleteSaved();
    void clear();

    void showGrammar();
    static void argsInfo();
    static void help();

    ~REPL();

private:
    polishd::Grammar m_grammar;
    polishd::Compiler* m_compiler;
    std::map<std::string, polishd::Function> m_functions;
    std::map<std::string, std::function<void()>> m_commands;
    const std::regex m_argsPattern;
};


#endif //INC_POLISHD_CALCULATOR_HPP
