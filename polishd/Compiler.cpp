#include <iostream>
#include <stack>
#include <bitset>
#include <sstream>

#include "Compiler.hpp"

namespace polishd {

    Compiler::Compiler(Grammar& grammar) : m_grammar(grammar) 
    {
    }

    const Grammar& Compiler::grammar() const
    {
        return m_grammar;
    }

    Function Compiler::compile(const std::string& infix)
    {
        bool expectOperand = true;
        std::list<Token> expression;
        std::stack<Token> stack;

        size_t length, i = 0;
        while (infix[i] == ' ')
            ++i;
        while (i < infix.size())
        {
            if(expectOperand) {
                if((length = Grammar::matchNumber(infix, i)))
                {
                    expression.push_back(Token{
                            .type = TokenType::Number,
                            .value = infix.substr(i, length)
                    });
                    expectOperand = false;
                }
                else if((length = m_grammar.matchPrefix(infix, i)))
                {
                    stack.push(Token{
                            .type = TokenType::Prefix,
                            .value = infix.substr(i, length)
                    });
                }
                else if((length = infix[i] == '('))
                {
                    stack.push(Token{.type = TokenType::Opening});
                }
                else if((length = Grammar::matchArgument(infix, i)))
                {
                    expression.push_back(Token{
                            .type = TokenType::Argument,
                            .value = infix.substr(i, length)
                    });
                    expectOperand = false;
                }
                else
                {
                    throw std::logic_error("Expected a number, an argument, a prefix function or an opening parenthesis");
                }
            }
            else
            {
                if((length = m_grammar.matchBinary(infix, i)))
                {
                    std::string signature = infix.substr(i, length);
                    Grammar::Precedence p = m_grammar.precedenceOf(signature);
                    while (!stack.empty()
                        && (stack.top().type == TokenType::Prefix || m_grammar.precedenceOf(stack.top().value) > p))
                    {
                        expression.push_back(stack.top());
                        stack.pop();
                    }
                    stack.push(Token{
                            .type = TokenType::Binary,
                            .value = signature
                    });
                    expectOperand = true;
                }
                else if ((length = m_grammar.matchPostfix(infix, i)))
                {
                    expression.push_back(Token{
                            .type = TokenType::Postfix,
                            .value = infix.substr(i, length)
                    });
                }
                else if ((length = (infix[i] == ')')))
                {
                    while (stack.top().type != TokenType::Opening)
                    {
                        expression.push_back(stack.top());
                        stack.pop();
                    }
                    stack.pop();
                }
                else
                {
                    throw std::logic_error("Expected a binary operator, a postfix function or a closing parenthesis");
                }
            }
            i += length;
            while (infix[i] == ' ')
                ++i;
        }
        while (!stack.empty())
        {
            expression.push_back(stack.top());
            stack.pop();
        }

        return Function(compile(expression),
                        infix,
                        stringify(expression));
    }

    std::list<Unit> Compiler::compile(const std::list<Token>& tokens)
    {
        std::list<Unit> expression;
        for (const auto& token: tokens)
        {
            switch (token.type)
            {
                case TokenType::Number:
                    expression.push_back(CompileNumber(token));
                    break;
                case TokenType::Argument:
                    expression.push_back(CompileArgument(token));
                    break;
                case TokenType::Prefix:
                    expression.push_back(CompilePrefix(token));
                    break;
                case TokenType::Binary:
                    expression.push_back(CompileBinary(token));
                    break;
                case TokenType::Postfix:
                    expression.push_back(CompilePostfix(token));
                    break;
                default:
                    std::stringstream s;
                    s << "Unhandled TokenType: " << static_cast<unsigned short>(token.type);
                    throw std::logic_error(s.str());
            }
        }
        return expression;
    }

    Unit Compiler::CompileNumber(const Token& token)
    {
        double x = stod(token.value);
        return [x](Stack stack, Function::Args args) -> double
        { return x; };
    }

    Unit Compiler::CompileArgument(const Token& token)
    {
        auto lookup = m_grammar.constants().find(token.value);
        if(lookup != m_grammar.constants().end()) {
            return [value=lookup->second](Stack stack, Function::Args args) -> double
            { return value; };
        }
        return [name=token.value](Stack stack, Function::Args args) -> double
        { return args.at(name); };
    }

    Unit Compiler::CompileUnary(const Token& token, const std::map<std::string, Grammar::Unary>& registry)
    {
        const Grammar::Unary& unary = registry.at(token.value);
        return [unary](Stack stack, Function::Args args) -> double
        {
            double x = stack.top();
            stack.pop();
            return unary(x);
        };
    }

    Unit Compiler::CompilePrefix(const Token& token)
    {
        return CompileUnary(token, m_grammar.prefix());
    }

    Unit Compiler::CompilePostfix(const Token& token)
    {
        return CompileUnary(token, m_grammar.postfix());
    }

    Unit Compiler::CompileBinary(const Token& token)
    {
        const Grammar::Binary& binary = m_grammar.binary().at(token.value).binary;
        return [binary](Stack stack, Function::Args args) -> double
        {
            double b = stack.top();
            stack.pop();
            double a = stack.top();
            stack.pop();
            return binary(a, b);
        };
    }

    std::string Compiler::stringify(const std::list<Token>& tokens)
    {
        std::stringstream stream;
        for(const Token& token : tokens)
            stream << token.value << ' ';
        return stream.str();
    }

} // namespace polishd
