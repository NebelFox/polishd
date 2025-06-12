#ifndef INC_POLISHD_GRAMMAR_HPP
#define INC_POLISHD_GRAMMAR_HPP

#include <string>
#include <string_view>

#include <TransparentStringKeyMap.hpp>

namespace polishd {

    class Grammar
    {
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

        static size_t matchNumber(const std::string& s, size_t start);
        static size_t matchArgument(const std::string& s, size_t start);

        size_t matchPrefix(const std::string& s, size_t start) const;
        size_t matchBinary(const std::string& s, size_t start) const;
        size_t matchPostfix(const std::string& s, size_t start) const;

        Precedence precedenceOf(const std::string& signature) const;

        void addConstant(const std::string& name, double value);
        void addPrefixOperator(const std::string& signature, Unary prefix);
        void addBinaryOperator(const std::string& signature, Binary binary, Precedence precedence);
        void addPostfixOperator(const std::string& signature, Unary postfix);

    private:
        TransparentStringKeyMap<double> m_constants;
        TransparentStringKeyMap<Unary> m_prefixOperators;
        TransparentStringKeyMap<BinaryOperator> m_binaryOperators;
        TransparentStringKeyMap<Unary> m_postfixOperators;
    private:
        template<typename Operator>
        static size_t match(const std::string& s, size_t start, const TransparentStringKeyMap<Operator>& ops);
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
