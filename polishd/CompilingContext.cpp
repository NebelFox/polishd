#include <CompilingContext.hpp>

#include <iostream>
#include <stack>
#include <sstream>
#include <charconv>

#include <exceptions.hpp>

namespace polishd {

    CompilingContext::CompilingContext(const Grammar& grammar, const std::string& infix) : m_grammar(grammar), m_infix(infix) 
    {
    }

    Function CompilingContext::compile()
    {
        TokenList tokens = tokenize();
        const size_t size = convert_infix_to_postfix(tokens);
        return Function(
            compile(tokens, size),
            m_arg_indices,
            m_infix,
            stringify(tokens)
        );
    }

    CompilingContext::TokenList CompilingContext::tokenize() const
    {
        size_t start = 0;
        TokenList tokens;
        auto last = tokens.before_begin();
        bool expectOperand = true;

        while (start < m_infix.size() && m_infix[start] == ' ')
            ++start;
        
        while (start < m_infix.size())
        {
            last = tokens.insert_after(last, 
                expectOperand
                ? parse_operand(start)
                : parse_operator(start));
            
            start += last->value.size();
            
            if(last->type == TokenType::Number || last->type == TokenType::Argument)
                expectOperand = false;
            else if(last->type == TokenType::Binary)
                expectOperand = true;

            while (start < m_infix.size() && m_infix[start] == ' ')
                ++start;
        }
        return tokens;
    }

    Token CompilingContext::parse_operand(size_t start) const
    {
        auto type = TokenType::None;
        size_t length;
        if((length = Grammar::match_number(m_infix, start)))
            type = TokenType::Number;
        else if((length = m_grammar.match_prefix(m_infix, start)))
            type = TokenType::Prefix;
        else if((length = m_infix[start] == '('))
            type = TokenType::Opening;
        else if((length = Grammar::match_argument(m_infix, start)))
            type = TokenType::Argument;
        else
            throw ExpressionSyntaxError("expected a number, an argument, a prefix function or an opening parenthesis starting at " + m_infix.substr(start));
        
        return Token{.type = type, .value = std::string_view(m_infix).substr(start, length)};
    }

    Token CompilingContext::parse_operator(size_t start) const
    {
        auto type = TokenType::None;
        size_t length;
        if((length = m_grammar.match_binary(m_infix, start)))
            type = TokenType::Binary;
        else if ((length = m_grammar.match_postfix(m_infix, start)))
            type = TokenType::Postfix;
        else if ((length = (m_infix[start] == ')')))
            type = TokenType::Closing;
        else
            throw ExpressionSyntaxError("expected a binary operator, a postfix function or a closing parenthesis starting at " + m_infix.substr(start));

        return Token{.type = type, .value = std::string_view(m_infix).substr(start, length)};
    }

    size_t CompilingContext::convert_infix_to_postfix(TokenList& infix) const
    {
        std::stack<Token> stack;
        size_t size = 0;
        auto prev = infix.before_begin();
        for(auto it = infix.begin(); it != infix.end();)
        {
            switch(it->type)
            {
                case TokenType::Prefix:
                case TokenType::Opening:
                    // move from list to stack
                    stack.push(*it);
                    it = infix.erase_after(prev);
                    break;

                case TokenType::Binary:
                {
                    // move from stack to list all prefix tokens 
                    // and binary tokens with higher precedence than current;
                    // then move current from list to stack
                    Token binary(*it);
                    infix.erase_after(prev);
                    const Grammar::Precedence p = m_grammar.precedence_of(binary.value);
                    while(!stack.empty() && (stack.top().type == TokenType::Prefix || m_grammar.precedence_of(stack.top().value) > p))
                    {
                        prev = infix.insert_after(prev, stack.top());
                        ++size;
                        stack.pop();
                    }
                    it = prev;
                    ++it;
                    stack.push(binary);
                    break;
                }

                case TokenType::Closing:
                {
                    infix.erase_after(prev);
                    while(stack.top().type != TokenType::Opening)
                    {
                        prev = infix.insert_after(prev, stack.top());
                        ++size;
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
                    ++size;
                    break;

            }
        }
        while(!stack.empty())
        {
            prev = infix.insert_after(prev, stack.top());
            ++size;
            stack.pop();
        }
        return size;
    }

    Function::Expression CompilingContext::compile(const TokenList& postfix, size_t size)
    {
        Function::Expression expression;
        expression.reserve(size);
        for (const Token& token: postfix)
            expression.push_back(compile(token));
        return expression;
    }

    Function::Unit CompilingContext::compile(const Token& token)
    {
        switch (token.type)
        {
            case TokenType::Number:
                return compile_number(token);
            case TokenType::Argument:
                return compile_argument(token);
            case TokenType::Prefix:
                return compile_prefix(token);
            case TokenType::Binary:
                return compile_binary(token);
            case TokenType::Postfix:
                return compile_postfix(token);
            default:
                throw UnexpectedTokenError(token);
        }
    }

    Function::Unit CompilingContext::compile_number(const Token& token)
    {
        double x;
        std::from_chars(token.value.data(), token.value.data() + token.value.size(), x);
        return {.type = token.type, .number = x};
    }

    Function::Unit CompilingContext::compile_argument(const Token& token)
    {
        if (const auto const_lookup = m_grammar.constants().find(token.value); const_lookup != m_grammar.constants().end())
        {
            return {.type = TokenType::Number, .number = const_lookup->second};
        }

        if (const auto arg_lookup = m_arg_indices.find(token.value); arg_lookup != m_arg_indices.end())
        {
            return {.type = token.type, .arg_index = arg_lookup->second};
        }

        m_arg_indices[token.value] = m_arg_indices.size();
        return {.type = token.type, .arg_index = m_arg_indices.size()-1};
    }
    
    Function::Unit CompilingContext::compile_prefix(const Token& token) const
    {
        return compile_unary(token, m_grammar.prefix());
    }
    
    Function::Unit CompilingContext::compile_postfix(const Token& token) const
    {
        return compile_unary(token, m_grammar.postfix());
    }
    
    Function::Unit CompilingContext::compile_unary(const Token& token, const TransparentStringKeyMap<Grammar::Unary>& ops)
    {
        const Grammar::Unary unary = ops.find(token.value)->second;
        return {.type = token.type, .unary = unary};
    }

    Function::Unit CompilingContext::compile_binary(const Token& token) const
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
