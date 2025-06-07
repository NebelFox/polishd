#ifndef INC_POLISHD_COMPILER_HPP
#define INC_POLISHD_COMPILER_HPP

#include "Grammar.hpp"
#include "Function.hpp"
#include "Token.hpp"

namespace polishd {

    class Compiler
    {
    public:
        explicit Compiler(Grammar& grammar);

        const Grammar& grammar() const;
        
        Function compile(const std::string& infix) const;

    private:
        std::list<Unit> compile(const std::list<Token>& tokens) const;

        static Unit CompileNumber(const Token& token);
        static Unit CompileUnary(const Token& token, const std::map<std::string, Grammar::Unary>& registry);

        Unit CompileArgument(const Token& token) const;
        Unit CompilePrefix(const Token& token) const;
        Unit CompilePostfix(const Token& token) const;
        Unit CompileBinary(const Token& token) const;

        static std::string stringify(const std::list<Token>& tokens);

    private:
        Grammar m_grammar;
    };

} // namespace polishd

#endif //INC_POLISHD_COMPILER_HPP
