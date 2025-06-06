#ifndef INC_POLISHD_CALCULATOR_HPP
#define INC_POLISHD_CALCULATOR_HPP

#include <iostream>
#include <regex>
#include <functional>

#include "Grammar.hpp"
#include "Compiler.hpp"
#include "Function.hpp"


using std::map;
using std::string;
using std::cin;
using std::cout;
using std::endl;

using std::regex;
using std::sregex_iterator;
using std::smatch;
using std::regex_match;


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
    map<string, Function> m_functions;
    map<string, std::function<void()>> m_commands;
    const regex m_argsPattern;
private:
    static void setupGrammar(Grammar& grammar);
};


#endif //INC_POLISHD_CALCULATOR_HPP
