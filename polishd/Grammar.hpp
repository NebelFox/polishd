#ifndef INC_POLISHD_GRAMMAR_HPP
#define INC_POLISHD_GRAMMAR_HPP

#include <string>
#include <string_view>

#include <TransparentStringKeyMap.hpp>

namespace polishd {

    class Grammar
    {
        friend class CompilingContext;

    public:
        typedef double (* Unary)(double);
        typedef double (* Binary)(double, double);
        
        using Precedence = unsigned char;
        
        struct BinaryOperator
        {
            Binary binary = nullptr;
            Precedence precedence = 0;
        };

    public:
        [[nodiscard]] const TransparentStringKeyMap<double>& constants() const;
        [[nodiscard]] const TransparentStringKeyMap<Unary>& prefix() const;
        [[nodiscard]] const TransparentStringKeyMap<BinaryOperator>& binary() const;
        [[nodiscard]] const TransparentStringKeyMap<Unary>& postfix() const;
        
        void add_constant(const std::string& name, double value);
        void add_prefix_operator(const std::string& signature, Unary prefix);
        void add_binary_operator(const std::string& signature, Binary binary, Precedence precedence);
        void add_postfix_operator(const std::string& signature, Unary postfix);
        
    private:
        static size_t match_number(const std::string& s, size_t start);
        static size_t match_argument(const std::string& s, size_t start);

        size_t match_prefix(const std::string& s, size_t start) const;
        size_t match_binary(const std::string& s, size_t start) const;
        size_t match_postfix(const std::string& s, size_t start) const;

        Precedence precedence_of(std::string_view signature) const;
        
        template<typename Operator>
        static size_t match(const std::string& s, size_t start, const TransparentStringKeyMap<Operator>& ops);
    private:
        TransparentStringKeyMap<double> m_constants;
        TransparentStringKeyMap<Unary> m_prefix_operators;
        TransparentStringKeyMap<BinaryOperator> m_binary_operators;
        TransparentStringKeyMap<Unary> m_postfix_operators;
    };


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

} // namespace polishd

#endif //INC_POLISHD_GRAMMAR_HPP
