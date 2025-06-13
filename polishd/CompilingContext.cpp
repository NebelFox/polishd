#include "CompilingContext.hpp"

#include <iostream>
#include <stack>
#include <sstream>
#include <charconv>

namespace polishd {

    CompilingContext::CompilingContext(const Grammar& grammar, const std::string& infix) : m_grammar(grammar), m_infix(infix) 
    {
    }

    Token CompilingContext::parseOperand(size_t start) const
    {
        auto type = TokenType::None;
        size_t length;
        if((length = Grammar::matchNumber(m_infix, start)))
            type = TokenType::Number;
        else if((length = m_grammar.matchPrefix(m_infix, start)))
            type = TokenType::Prefix;
        else if((length = m_infix[start] == '('))
            type = TokenType::Opening;
        else if((length = Grammar::matchArgument(m_infix, start)))
            type = TokenType::Argument;
        else
            throw std::logic_error("Expected a number, an argument, a prefix function or an opening parenthesis starting at " + m_infix.substr(start));
        
        return Token{.type = type, .value = std::string_view(m_infix).substr(start, length)};
    }

    Token CompilingContext::parseOperator(size_t start) const
    {
        auto type = TokenType::None;
        size_t length;
        if((length = m_grammar.matchBinary(m_infix, start)))
            type = TokenType::Binary;
        else if ((length = m_grammar.matchPostfix(m_infix, start)))
            type = TokenType::Postfix;
        else if ((length = (m_infix[start] == ')')))
            type = TokenType::Closing;
        else
            throw std::logic_error("Expected a binary operator, a postfix function or a closing parenthesis starting at " + m_infix.substr(start));

        return Token{.type = type, .value = std::string_view(m_infix).substr(start, length)};
    }

    CompilingContext::TokenList CompilingContext::tokenize() const
    {
        size_t start = 0;
        TokenList tokens;
        auto last = tokens.before_begin();
        bool expectOperand = true;

        while (m_infix[start] == ' ')
            ++start;
        
        while (start < m_infix.size())
        {
            last = tokens.insert_after(last, 
                expectOperand
                ? parseOperand(start)
                : parseOperator(start));
            
            start += last->value.size();
            
            if(last->type == TokenType::Number || last->type == TokenType::Argument)
                expectOperand = false;
            else if(last->type == TokenType::Binary)
                expectOperand = true;

            while (m_infix[start] == ' ')
                ++start;
        }
        return tokens;
    }

    void CompilingContext::convertInfixToPostfix(TokenList& infix) const
    {
        std::stack<Token> stack;

        auto prev = infix.before_begin();
        auto it = infix.begin();
        for(; it != infix.end();)
        {
            switch(it->type)
            {
                case TokenType::Prefix:
                case TokenType::Opening:
                    // move from list to stack
                    stack.push(std::move(*it));
                    it = infix.erase_after(prev);
                    break;

                case TokenType::Binary:
                {
                    // move from stack to list all prefix tokens 
                    // and binary tokens with higher precedence than current;
                    // then move current from list to stack
                    Token binary(std::move(*it));
                    it = infix.erase_after(prev);
                    Grammar::Precedence p = m_grammar.precedenceOf(binary.value);
                    while(!stack.empty() && (stack.top().type == TokenType::Prefix || m_grammar.precedenceOf(stack.top().value) > p))
                    {
                        prev = infix.insert_after(prev, stack.top());
                        stack.pop();
                    }
                    it = prev;
                    ++it;
                    stack.push(std::move(binary));
                    break;
                }

                case TokenType::Closing:
                {
                    it = infix.erase_after(prev);
                    while(stack.top().type != TokenType::Opening)
                    {
                        prev = infix.insert_after(prev, std::move(stack.top()));
                        stack.pop();
                    }
                    it = prev;
                    ++it;
                    stack.pop(); // pop Opening token from stack
                    break;
                }

                default:
                    prev = it;
                    ++it;
                    break;

            }
        }
        while(!stack.empty())
        {
            prev = infix.insert_after(prev, stack.top());
            stack.pop();
        }
    }

    Function CompilingContext::compile()
    {
        TokenList tokens = tokenize();
        convertInfixToPostfix(tokens);
        return Function(
            compile(tokens),
            m_argIndices,
            m_infix,
            stringify(tokens)
        );
    }

    Unit CompilingContext::compile(const Token& token)
    {
        switch (token.type)
        {
            case TokenType::Number:
                return CompileNumber(token);
                break;
            case TokenType::Argument:
                return CompileArgument(token);
                break;
            case TokenType::Prefix:
                return CompilePrefix(token);
                break;
            case TokenType::Binary:
                return CompileBinary(token);
                break;
            case TokenType::Postfix:
                return CompilePostfix(token);
                break;
            default:
                std::stringstream s;
                s << "Unhandled TokenType: " << static_cast<unsigned short>(token.type);
                throw std::logic_error(s.str());
        }
    }

    UnitList CompilingContext::compile(const TokenList& postfix)
    {
        std::forward_list<Unit> expression;
        auto it = expression.before_begin();
        for (const Token& token: postfix)
        {
            it = expression.insert_after(it, compile(token));
        }
        return expression;
    }

    Unit CompilingContext::CompileNumber(const Token& token)
    {
        double x;
        std::from_chars(token.value.data(), token.value.data() + token.value.size(), x);
        return {.type = token.type, .number = x};
    }

    Unit CompilingContext::CompileArgument(const Token& token)
    {
        if (const auto lookup = m_grammar.constants().find(token.value); lookup != m_grammar.constants().end()) {
            return {.type = TokenType::Number, .number = lookup->second};
        }
        if (const auto argLookup = m_argIndices.find(token.value); argLookup != m_argIndices.end())
        {
            return {.type = token.type, .argIndex = argLookup->second};
        }
        m_argIndices[std::string(token.value)] = m_argIndices.size();
        return {.type = token.type, .argIndex = m_argIndices.size()-1};
    }

    Unit CompilingContext::CompileUnary(const Token& token, const TransparentStringKeyMap<Grammar::Unary>& registry)
    {
        const Grammar::Unary unary = registry.find(token.value)->second;
        return {.type = token.type, .unary = unary};
    }

    Unit CompilingContext::CompilePrefix(const Token& token) const
    {
        return CompileUnary(token, m_grammar.prefix());
    }

    Unit CompilingContext::CompilePostfix(const Token& token) const
    {
        return CompileUnary(token, m_grammar.postfix());
    }

    Unit CompilingContext::CompileBinary(const Token& token) const
    {
        const Grammar::Binary binary = m_grammar.binary().find(token.value)->second.binary;
        return {.type = token.type, .binary = binary};
    }

    std::string CompilingContext::stringify(const TokenList& tokens)
    {
        std::stringstream stream;
        for(const Token& token : tokens)
            stream << token.value << ' ';
        return stream.str();
    }

} // namespace polishd
