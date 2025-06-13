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

        Token parseOperand(size_t start) const;
        Token parseOperator(size_t start) const;
        TokenList tokenize() const;

        void convertInfixToPostfix(TokenList& infix) const;
        
        UnitList compile(const TokenList& postfix);
        Unit compile(const Token& token);

        static Unit CompileNumber(const Token& token);
        static Unit CompileUnary(const Token& token, const TransparentStringKeyMap<Grammar::Unary>& registry);
        Unit CompileArgument(const Token& token);
        Unit CompilePrefix(const Token& token) const;
        Unit CompilePostfix(const Token& token) const;
        Unit CompileBinary(const Token& token) const;

        static std::string stringify(const TokenList& tokens);
    private:
        const Grammar& m_grammar;
        const std::string& m_infix;
        std::unordered_map<std::string_view, size_t> m_argIndices;
    };

} // namespace polishd

#endif //INC_POLISHD_COMPILER_HPP
