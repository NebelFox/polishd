#include "Function.hpp"

namespace polishd {

    Function::Function(const std::list<Unit>& expression,
                    const std::string& infix,
                    const std::string& postfix)
    {
        m_expression = expression;
        m_infix = infix;
        m_postfix = postfix;
    }

    const std::string& Function::postfix() const
    {
        return m_postfix;
    }

    const std::string& Function::infix() const
    {
        return m_infix;
    }

    double Function::evaluate(const Args& args) const
    {
        std::stack<double> stack;
        for (const Unit& unit: m_expression)
            stack.push(unit(stack, args));
        return stack.top();
    }

    double Function::evaluate() const
    {
        Args args;
        return evaluate(args);
    }

    double Function::operator()(const Args& args) const
    {
        return evaluate(args);
    }

    double Function::operator()() const
    {
        return evaluate();
    }

} // namespace polishd
