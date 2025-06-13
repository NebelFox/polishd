#ifndef INC_POLISHD_FUNCTION_HPP
#define INC_POLISHD_FUNCTION_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <forward_list>
#include <stack>
#include <vector>

#include <TransparentStringKeyMap.hpp>
#include "Token.hpp"
#include <Grammar.hpp>

namespace polishd {

    using Stack = std::stack<double>;
    using Args = TransparentStringKeyMap<double>;
    struct Unit {
        // On x64
        // The union takes 8 bytes
        // But `TokenType` takes 1 byte
        // So the whole Unit structure takes 16 bytes instead of 9 due to padding (multiples of 8)
        TokenType type;
        union {
            double number;
            Grammar::Unary unary;
            Grammar::Binary binary;
            size_t arg_index;
        };
    };
    using UnitList = std::forward_list<Unit>;

    class Function
    {
    public:
        explicit Function(UnitList expression,
                          const std::unordered_map<std::string_view, size_t>& arg_indices,
                          const std::string& infix,
                          std::string postfix);
        
        [[nodiscard]] double evaluate(const Args& args) const;
        [[nodiscard]] double evaluate() const;

        double operator()(const Args& args) const;
        double operator()() const;

        const std::string& infix() const;
        const std::string& postfix() const;

    private:
        UnitList m_expression;
        std::vector<std::string_view> m_arg_names;
        std::string m_infix;
        std::string m_postfix;
    };

} // namespace polishd

#endif //INC_POLISHD_FUNCTION_HPP
