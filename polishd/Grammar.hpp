#ifndef INC_POLISHD_GRAMMAR_HPP
#define INC_POLISHD_GRAMMAR_HPP

#include <string>
#include <unordered_map>

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
        [[nodiscard]] const std::unordered_map<std::string, double>& constants() const;
        [[nodiscard]] const std::unordered_map<std::string, Unary>& prefix() const;
        [[nodiscard]] const std::unordered_map<std::string, BinaryOperator>& binary() const;
        [[nodiscard]] const std::unordered_map<std::string, Unary>& postfix() const;

        static size_t matchNumber(const std::string& s, const size_t start);
        static size_t matchArgument(const std::string& s, const size_t start);

        size_t matchPrefix(const std::string& s, const size_t start) const;
        size_t matchBinary(const std::string& s, const size_t start) const;
        size_t matchPostfix(const std::string& s, const size_t start) const;

        Precedence precedenceOf(const std::string& signature) const;

        void addConstant(const std::string& name, const double value);
        void addPrefixOperator(const std::string& signature, Unary prefix);
        void addBinaryOperator(const std::string& signature, Binary binary, const Precedence precedence);
        void addPostfixOperator(const std::string& signature, Unary postfix);

    private:
        std::unordered_map<std::string, double> m_constants;
        std::unordered_map<std::string, Unary> m_prefixOperators;
        std::unordered_map<std::string, BinaryOperator> m_binaryOperators;
        std::unordered_map<std::string, Unary> m_postfixOperators;
    private:
        template<typename Operator>
        static size_t match(const std::string& s, const size_t start, const std::unordered_map<std::string, Operator>& ops);
    };


    template<typename Operator>
    size_t Grammar::match(const std::string& s, const size_t start, const std::unordered_map<std::string, Operator>& ops)
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

} // namespace polishd

#endif //INC_POLISHD_GRAMMAR_HPP
