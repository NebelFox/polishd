#include "Function.hpp"

#include <iostream>

namespace polishd {

    Function::Function(UnitList expression,
                       const std::unordered_map<std::string_view, size_t>& arg_indices,
                       const std::string& infix,
                       std::string postfix)
        : m_expression(std::move(expression)),
          m_arg_names(arg_indices.size()),
          m_infix(infix),
          m_postfix(std::move(postfix))
    {
        for(const auto& [arg_name, index] : arg_indices)
        {
            // translate arg name string_views to point to m_infix
            const size_t start = arg_name.data() - infix.data();
            m_arg_names[index] = std::string_view(m_infix).substr(start, arg_name.size());
        }
    }

    double Function::evaluate(const Args& args) const
    {
        // prepare argument values
        std::vector<double> arg_values;
        arg_values.reserve(m_arg_names.size());
        for(size_t i=0; i<m_arg_names.size(); ++i)
        {
            auto lookup = args.find(m_arg_names[i]);
            if(lookup == args.end())
                throw "Missing argument " + std::string(m_arg_names[i]);
            arg_values.push_back(lookup->second);
        }
        // evaluate
        Stack stack;
        double a, b;
        for (const Unit unit: m_expression)
        {
            switch(unit.type)
            {
                case TokenType::Number:
                    stack.push(unit.number);
                    break;
                case TokenType::Prefix:
                case TokenType::Postfix:
                    a = stack.top();
                    stack.pop();
                    stack.push(unit.unary(a));
                    break;
                case TokenType::Binary:
                    b = stack.top();
                    stack.pop();
                    a = stack.top();
                    stack.pop();
                    stack.push(unit.binary(a, b));
                    break;
                case TokenType::Argument:
                    stack.push(arg_values[unit.arg_index]);
                    break;
                default:
                    throw "Unexpected token type";
                    break;

            }
        }
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

    const std::string& Function::infix() const
    {
        return m_infix;
    }

    const std::string& Function::postfix() const
    {
        return m_postfix;
    }

} // namespace polishd
