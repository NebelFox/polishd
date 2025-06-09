#ifndef INC_POLISHD_FUNCTION_HPP
#define INC_POLISHD_FUNCTION_HPP

#include "Token.hpp"

#include <string>
#include <unordered_map>
#include <list>
#include <functional>

namespace polishd {

    using Stack = std::stack<double>;
    using Args = std::unordered_map<std::string, double>;
    using Unit = std::function<double (Stack&, const Args&)>;

    class Function
    {
    public:
        explicit Function(const std::list<Unit>& expression,
                          const std::string& infix,
                          const std::string& postfix);
        
        [[nodiscard]] double evaluate(const Args& args) const;
        [[nodiscard]] double evaluate() const;

        double operator()(const Args& args) const;
        double operator()() const;

        const std::string& infix() const;
        const std::string& postfix() const;

    private:
        std::list<Unit> m_expression;
        std::string m_infix;
        std::string m_postfix;
    };

} // namespace polishd

#endif //INC_POLISHD_FUNCTION_HPP
