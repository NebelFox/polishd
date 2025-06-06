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
    REPL();

    void start();

    void eval();
    void save();
    void show();
    void evalSaved();
    void listSaved();
    void deleteSaved();
    void clear();

    void grammar();
    static void argsInfo();
    static void help();

    ~REPL();

private:
    Grammar m_grammar;
    Compiler* m_compiler;
    std::map<std::string, Function> m_functions;
    std::map<std::string, std::function<void()>> m_commands;
    const std::regex m_argsPattern;
private:
    static void setupGrammar(Grammar& grammar);
};


#endif //INC_POLISHD_CALCULATOR_HPP
