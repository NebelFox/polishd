#ifndef INC_POLISHD_COMPILER_HPP
#define INC_POLISHD_COMPILER_HPP

#include <string>
#include <string_view>
#include <forward_list>
#include <unordered_map>

#include "Token.hpp"
#include "Grammar.hpp"
#include "Function.hpp"

namespace polishd {

    class CompilingContext
    {
    public:
        explicit CompilingContext(const Grammar& grammar, const std::string& infix);
        
        Function compile();

    private:
        using TokenList = std::forward_list<Token>;

        Token parse_operand(size_t start) const;
        Token parse_operator(size_t start) const;
        TokenList tokenize() const;

        size_t convert_infix_to_postfix(TokenList& infix) const;
        
        Expression compile(const TokenList& postfix, size_t size);
        Unit compile(const Token& token);

        static Unit compile_number(const Token& token);
        static Unit compile_unary(const Token& token, const TransparentStringKeyMap<Grammar::Unary>& registry);
        Unit compile_argument(const Token& token);
        Unit compile_prefix(const Token& token) const;
        Unit compile_postfix(const Token& token) const;
        Unit compile_binary(const Token& token) const;

        static std::string stringify(const TokenList& tokens);
    private:
        const Grammar& m_grammar;
        const std::string& m_infix;
        std::unordered_map<std::string_view, size_t> m_arg_indices;
    };

} // namespace polishd

#endif //INC_POLISHD_COMPILER_HPP
