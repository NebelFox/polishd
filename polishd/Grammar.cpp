#include "Grammar.hpp"

const map<string, double>& Grammar::constants() const
{
    return m_constants;
}

const map<string, Grammar::Unary>& Grammar::prefix() const
{
    return m_prefixOperators;
}

const map<string, Grammar::BinaryOperator>& Grammar::binary() const
{
    return m_binaryOperators;
}

const map<string, Grammar::Unary>& Grammar::postfix() const
{
    return m_postfixOperators;
}

size_t Grammar::matchNumber(const string& s, const size_t start)
{
    bool isSigned = (s[start] == '-' || s[start] == '+');
    size_t length = isSigned;
    while (isdigit(s[start + length]))
        ++length;
    length += s[start + length] == '.';
    while (isdigit(s[start + length]))
        ++length;
    return length * (!isSigned || length > 1);
}

size_t Grammar::matchArgument(const string& s, const size_t start)
{
    size_t length = 0;
    while (isalpha(s[start + length]) || s[start + length] == '_')
        ++length;
    return length;
}

size_t Grammar::matchPrefix(const string& s, const size_t start) const
{
    return match(s, start, m_prefixOperators);
}

size_t Grammar::matchBinary(const string& s, const size_t start) const
{
    return match(s, start, m_binaryOperators);
}

size_t Grammar::matchPostfix(const string& s, const size_t start) const
{
    return match(s, start, m_postfixOperators);
}

Grammar::Precedence Grammar::precedenceOf(const string& signature) const
{
    auto lookup = m_binaryOperators.find(signature);
    return (lookup != m_binaryOperators.end()) ? lookup->second.precedence : 0;
}

void Grammar::addConstant(const string& name, const double value)
{
    m_constants.insert_or_assign(name, value);
}

void Grammar::addPrefixOperator(const string& signature, Unary prefix)
{
    m_prefixOperators.insert_or_assign(signature, prefix);
}

void Grammar::addBinaryOperator(const string& signature, Binary binary, const Precedence precedence)
{
    m_binaryOperators.insert_or_assign(signature, BinaryOperator {binary, precedence});
}

void Grammar::addPostfixOperator(const string& signature, Unary postfix)
{
    m_postfixOperators.insert_or_assign(signature, postfix);
}
