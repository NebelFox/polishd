#ifndef INC_POLISHD_FUNCTION_HPP
#define INC_POLISHD_FUNCTION_HPP


#include <string>
#include <map>
#include <list>

#include "Token.hpp"

namespace polishd {

    class Function
    {
        friend class Compiler;
    public:
        using Args = std::map<std::string, double>;
        
        explicit Function(const std::list<Unit>& expression,
                        const std::string& infix,
                        const std::string& postfix);

        
        [[nodiscard]] const std::string& postfix() const;    
        [[nodiscard]] const std::string& infix() const;
        
        [[nodiscard]] double evaluate(const Args& args) const;
        [[nodiscard]] double evaluate() const;

        double operator()(const Args& args) const;
        double operator()() const;

    private:
        std::string m_infix;
        std::string m_postfix;
        std::list<Unit> m_expression;
    };

} // namespace polishd

#endif //INC_POLISHD_FUNCTION_HPP
