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
    explicit REPL(const polishd::Grammar& grammar);

    void start();

    void eval() const;
    void save();
    void show() const;
    void show_saved() const;
    void eval_saved() const;
    void list_saved() const;
    void delete_saved();
    void clear();

    void show_grammar() const;
    static void help();

private:
    static std::pair<size_t, polishd::Args> parse_args(const std::string& s);
    static void show_function(const polishd::Function& f);
private:
    const polishd::Grammar& m_grammar;
    std::unordered_map<std::string, polishd::Function> m_functions;
    std::unordered_map<std::string, std::function<void()>> m_commands;
    static const std::regex s_args_pattern;
};

#endif //INC_POLISHD_CALCULATOR_HPP
