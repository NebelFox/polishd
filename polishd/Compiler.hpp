#ifndef INC_POLISHD_COMPILER_HPP
#define INC_POLISHD_COMPILER_HPP

#include <string>
#include <forward_list>
#include <unordered_map>

#include "Token.hpp"
#include <TransparentStringKeyMap.hpp>
#include "Grammar.hpp"
#include "Function.hpp"

namespace polishd {

    class Compiler
    {
    public:
        explicit Compiler(Grammar& grammar);

        const Grammar& grammar() const;
        
        Function compile(const std::string& infix) const;

    private:
        using TokenList = std::forward_list<Token>;

        Token parseOperand(const std::string& infix, size_t start) const;
        Token parseOperator(const std::string& infix, size_t start) const;
        TokenList tokenize(const std::string& infix) const;

        void convertInfixToPostfix(TokenList& infix) const;
        
        UnitList compile(const TokenList& postfix) const;
        Unit compile(const Token& token) const;

        static Unit CompileNumber(const Token& token);
        static Unit CompileUnary(const Token& token, const TransparentStringKeyMap<Grammar::Unary>& registry);
        Unit CompileArgument(const Token& token) const;
        Unit CompilePrefix(const Token& token) const;
        Unit CompilePostfix(const Token& token) const;
        Unit CompileBinary(const Token& token) const;

        static std::string stringify(const TokenList& tokens);

    private:
        Grammar m_grammar;
    };

} // namespace polishd

#endif //INC_POLISHD_COMPILER_HPP
