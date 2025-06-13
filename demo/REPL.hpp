#ifndef INC_POLISHD_CALCULATOR_HPP
#define INC_POLISHD_CALCULATOR_HPP

#include <string>
#include <unordered_map>
#include <regex>
#include <functional>

#include <polishd.hpp>

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
    static std::pair<size_t, polishd::Args> parseArgs(const std::string& s); 
private:
    const polishd::Grammar& m_grammar;
    std::unordered_map<std::string, polishd::Function> m_functions;
    std::unordered_map<std::string, std::function<void()>> m_commands;
    static const std::regex s_argsPattern;
};

#endif //INC_POLISHD_CALCULATOR_HPP
