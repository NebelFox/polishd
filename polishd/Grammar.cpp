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

    size_t Grammar::match_number(const std::string& s, size_t start)
    {
        const bool is_signed = (s[start] == '-' || s[start] == '+');
        size_t end = start + is_signed;
        while (end < s.size() && isdigit(s[end]))
            ++end;
        end += end < s.size() && s[end] == '.';
        while (end < s.size() && isdigit(s[end]))
            ++end;
        return (end - start) * (!is_signed || (end - start) > 1);
    }

    size_t Grammar::match_argument(const std::string& s, size_t start)
    {
        size_t end = start;
        while (end < s.size() && (isalpha(s[end]) || s[end] == '_'))
            ++end;
        return end - start;
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

    template<typename Operator>
    size_t Grammar::match(const std::string& s, size_t start, const TransparentStringKeyMap<Operator>& ops)
    {
        for (const auto& pair: ops)
        {
            if (s.compare(start, pair.first.length(), pair.first) == 0)
                return pair.first.length();
        }
        return 0;
    }

    Grammar::Precedence Grammar::precedence_of(std::string_view signature) const
    {
        const auto lookup = m_binary_operators.find(signature);
        return lookup != m_binary_operators.end() ? lookup->second.precedence : 0;
    }

} // namespace polishd
