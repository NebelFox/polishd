#include <iostream>
#include <stack>
#include <bitset>
#include <sstream>

#include "Compiler.hpp"

namespace polishd {

    Compiler::Compiler(Grammar& grammar) : m_grammar(grammar) 
    {
    }

    Function Compiler::compile(const std::string& infix)
    {
        static const TokenType
                operands = static_cast<TokenType> (TT_NUMBER
                                                | TT_ARGUMENT
                                                | TT_PREFIX
                                                | TT_OPEN);
        static const TokenType
                operators = static_cast<TokenType> (TT_BINARY
                                                | TT_POSTFIX
                                                | TT_CLOSE);

        TokenType next = operands;
        std::list<Token> expression;
        std::stack<Token> stack;

        size_t length, i = 0;
        while (infix[i] == ' ')
            ++i;
        while (i < infix.size())
        {
            if ((TT_NUMBER & next) && (length = Grammar::matchNumber(infix, i)))
            {
                expression.push_back(Token{
                        .type = TT_NUMBER,
                        .value = infix.substr(i, length)
                });
                next = operators;
            }
            else if ((TT_PREFIX & next) && (length = m_grammar.matchPrefix(infix, i)))
            {
                stack.push(Token{
                        .type = TT_PREFIX,
                        .value = infix.substr(i, length)
                });
                next = operands;
            }
            else if ((TT_BINARY & next) && (length = m_grammar.matchBinary(infix, i)))
            {
                std::string signature = infix.substr(i, length);
                Grammar::Precedence p = m_grammar.precedenceOf(signature);
                while (!stack.empty()
                    && (stack.top().type == TT_PREFIX || m_grammar.precedenceOf(stack.top().value) > p))
                {
                    expression.push_back(stack.top());
                    stack.pop();
                }
                stack.push(Token{
                        .type = TT_BINARY,
                        .value = signature
                });
                next = operands;
            }
            else if ((TT_POSTFIX & next) && (length = m_grammar.matchPostfix(infix, i)))
            {
                expression.push_back(Token{
                        .type = TT_POSTFIX,
                        .value = infix.substr(i, length)
                });
                next = operators;
            }
            else if ((TT_OPEN & next) && (length = infix[i] == '('))
            {
                stack.push(Token{.type = TT_OPEN});
                next = operands;
            }
            else if ((TT_CLOSE & next) && (length = (infix[i] == ')')))
            {
                while (stack.top().type != TT_OPEN)
                {
                    expression.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
                next = operators;
            }
            else if ((TT_ARGUMENT & next) && (length = Grammar::matchArgument(infix, i)))
            {
                expression.push_back(Token{
                        .type = TT_ARGUMENT,
                        .value = infix.substr(i, length)
                });
                next = operators;
            }
            else
            {
                std::bitset<8> b(next);
                std::stringstream s;
                s << "Unexpected token at " << i << "; UnitGroup: " << b;
                throw std::logic_error(s.str());
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
                case TT_NUMBER:
                    expression.push_back(CompileNumber(token));
                    break;
                case TT_ARGUMENT:
                    expression.push_back(CompileArgument(token));
                    break;
                case TT_PREFIX:
                    expression.push_back(CompilePrefix(token));
                    break;
                case TT_BINARY:
                    expression.push_back(CompileBinary(token));
                    break;
                case TT_POSTFIX:
                    expression.push_back(CompilePostfix(token));
                    break;
                default:
                    std::stringstream s;
                    s << "Unhandled TokenType: " << token.type;
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
