#include <Grammar.hpp>

namespace polishd {

    const TransparentStringKeyMap<double>& Grammar::constants() const
    {
        return m_constants;
    }

    const TransparentStringKeyMap<Grammar::Unary>& Grammar::prefix() const
    {
        return m_prefix_operators;
    }

    const TransparentStringKeyMap<Grammar::BinaryOperator>& Grammar::binary() const
    {
        return m_binary_operators;
    }

    const TransparentStringKeyMap<Grammar::Unary>& Grammar::postfix() const
    {
        return m_postfix_operators;
    }

    size_t Grammar::match_number(const std::string& s, size_t start)
    {
        bool is_signed = (s[start] == '-' || s[start] == '+');
        size_t length = is_signed;
        while (length < s.size() && isdigit(s[start + length]))
            ++length;
        length += length < s.size() && s[start + length] == '.';
        while (length < s.size() && isdigit(s[start + length]))
            ++length;
        return length * (!is_signed || length > 1);
    }

    size_t Grammar::match_argument(const std::string& s, size_t start)
    {
        size_t length = 0;
        while (length < s.size() && (isalpha(s[start + length]) || s[start + length] == '_'))
            ++length;
        return length;
    }

    size_t Grammar::match_prefix(const std::string& s, size_t start) const
    {
        return match(s, start, m_prefix_operators);
    }

    size_t Grammar::match_binary(const std::string& s, size_t start) const
    {
        return match(s, start, m_binary_operators);
    }

    size_t Grammar::match_postfix(const std::string& s, size_t start) const
    {
        return match(s, start, m_postfix_operators);
    }

    Grammar::Precedence Grammar::precedence_of(std::string_view signature) const
    {
        auto lookup = m_binary_operators.find(signature);
        return (lookup != m_binary_operators.end()) ? lookup->second.precedence : 0;
    }

    void Grammar::add_constant(const std::string& name, double value)
    {
        m_constants.insert_or_assign(name, value);
    }

    void Grammar::add_prefix_operator(const std::string& signature, Unary prefix)
    {
        m_prefix_operators.insert_or_assign(signature, prefix);
    }

    void Grammar::add_binary_operator(const std::string& signature, Binary binary, Precedence precedence)
    {
        m_binary_operators.insert_or_assign(signature, BinaryOperator {binary, precedence});
    }

    void Grammar::add_postfix_operator(const std::string& signature, Unary postfix)
    {
        m_postfix_operators.insert_or_assign(signature, postfix);
    }

} // namespace polishd
