#ifndef INC_POLISHD_GRAMMAR_HPP
#define INC_POLISHD_GRAMMAR_HPP

#include <string>
#include <map>

using std::string;
using std::map;

class Grammar
{
public:
    typedef double (* Unary)(const double);
    typedef double (* Binary)(const double, const double);
    
    using Precedence = unsigned char;
    
    struct BinaryOperator
    {
        Binary binary = nullptr;
        Precedence precedence = 0;
    };

public:
    [[nodiscard]] const map<string, double>& constants() const;
    [[nodiscard]] const map<string, Unary>& prefix() const;
    [[nodiscard]] const map<string, BinaryOperator>& binary() const;
    [[nodiscard]] const map<string, Unary>& postfix() const;

    static size_t matchNumber(const string& s, const size_t start);
    static size_t matchArgument(const string& s, const size_t start);

    size_t matchPrefix(const string& s, const size_t start) const;
    size_t matchBinary(const string& s, const size_t start) const;
    size_t matchPostfix(const string& s, const size_t start) const;

    Precedence precedenceOf(const string& signature) const;

    void addConstant(const string& name, const double value);
    void addPrefixOperator(const string& signature, Unary prefix);
    void addBinaryOperator(const string& signature, Binary binary, const Precedence precedence);
    void addPostfixOperator(const string& signature, Unary postfix);

private:
    map<string, double> m_constants;
    map<string, Unary> m_prefixOperators;
    map<string, BinaryOperator> m_binaryOperators;
    map<string, Unary> m_postfixOperators;
private:
    template<typename Operator>
    static size_t match(const string& s, const size_t start, const map<string, Operator>& ops);
};


template<typename Operator>
size_t Grammar::match(const string& s, const size_t start, const map<string, Operator>& ops)
{
    size_t length;
    for (const auto& pair: ops)
    {
        length = pair.first.length();
        if (start + length <= s.length() && s.compare(start, length, pair.first) == 0)
        {
            return length;
        }
    }
    return 0;
}


#endif //INC_POLISHD_GRAMMAR_HPP
