#include "Grammar.hpp"

namespace polishd {

    const std::unordered_map<std::string, double>& Grammar::constants() const
    {
        return m_constants;
    }

    const std::unordered_map<std::string, Grammar::Unary>& Grammar::prefix() const
    {
        return m_prefixOperators;
    }

    const std::unordered_map<std::string, Grammar::BinaryOperator>& Grammar::binary() const
    {
        return m_binaryOperators;
    }

    const std::unordered_map<std::string, Grammar::Unary>& Grammar::postfix() const
    {
        return m_postfixOperators;
    }

    size_t Grammar::matchNumber(const std::string& s, const size_t start)
    {
        bool isSigned = (s[start] == '-' || s[start] == '+');
        size_t length = isSigned;
        while (length < s.size() && isdigit(s[start + length]))
            ++length;
        length += length < s.size() && s[start + length] == '.';
        while (length < s.size() && isdigit(s[start + length]))
            ++length;
        return length * (!isSigned || length > 1);
    }

    size_t Grammar::matchArgument(const std::string& s, const size_t start)
    {
        size_t length = 0;
        while (length < s.size() && (isalpha(s[start + length]) || s[start + length] == '_'))
            ++length;
        return length;
    }

    size_t Grammar::matchPrefix(const std::string& s, const size_t start) const
    {
        return match(s, start, m_prefixOperators);
    }

    size_t Grammar::matchBinary(const std::string& s, const size_t start) const
    {
        return match(s, start, m_binaryOperators);
    }

    size_t Grammar::matchPostfix(const std::string& s, const size_t start) const
    {
        return match(s, start, m_postfixOperators);
    }

    Grammar::Precedence Grammar::precedenceOf(const std::string& signature) const
    {
        auto lookup = m_binaryOperators.find(signature);
        return (lookup != m_binaryOperators.end()) ? lookup->second.precedence : 0;
    }

    void Grammar::addConstant(const std::string& name, const double value)
    {
        m_constants.insert_or_assign(name, value);
    }

    void Grammar::addPrefixOperator(const std::string& signature, Unary prefix)
    {
        m_prefixOperators.insert_or_assign(signature, prefix);
    }

    void Grammar::addBinaryOperator(const std::string& signature, Binary binary, const Precedence precedence)
    {
        m_binaryOperators.insert_or_assign(signature, BinaryOperator {binary, precedence});
    }

    void Grammar::addPostfixOperator(const std::string& signature, Unary postfix)
    {
        m_postfixOperators.insert_or_assign(signature, postfix);
    }

} // namespace polishd
