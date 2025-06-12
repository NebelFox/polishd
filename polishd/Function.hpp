#ifndef INC_POLISHD_FUNCTION_HPP
#define INC_POLISHD_FUNCTION_HPP

#include <string>
#include <unordered_map>
#include <forward_list>
#include <functional>
#include <stack>

#include <TransparentStringKeyMap.hpp>
#include "Token.hpp"

namespace polishd {

    using Stack = std::stack<double>;
    using Args = TransparentStringKeyMap<double>;
    using Unit = std::function<double (Stack&, const Args&)>;
    using UnitList = std::forward_list<Unit>;

    class Function
    {
    public:
        explicit Function(const UnitList& expression,
                          const std::string& infix,
                          const std::string& postfix);
        
        [[nodiscard]] double evaluate(const Args& args) const;
        [[nodiscard]] double evaluate() const;

        double operator()(const Args& args) const;
        double operator()() const;

        const std::string& infix() const;
        const std::string& postfix() const;

    private:
        UnitList m_expression;
        std::string m_infix;
        std::string m_postfix;
    };

} // namespace polishd

#endif //INC_POLISHD_FUNCTION_HPP
