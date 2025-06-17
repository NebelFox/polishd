#ifndef INC_POLISHD_EXCEPTIONS_HPP
#define INC_POLISHD_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

#include <Token.hpp>

namespace polishd {

    class Exception : public std::runtime_error
    {
    public:
        explicit Exception(const std::string& what);
    };

    class MissingArgumentError : public Exception
    {
    public:
        explicit MissingArgumentError(const std::string& arg_name);
    };

    class ExpressionSyntaxError : public Exception
    {
    public:
        explicit ExpressionSyntaxError(const std::string& what);
    };

    class UnexpectedTokenError : public Exception
    {
    public:
        explicit UnexpectedTokenError(const Token& token);
    
    private:
        static std::string make_message(const Token& token);
    };
    
    class UnexpectedUnitError : public Exception
    {
    public:
        explicit UnexpectedUnitError(TokenType type);

    private:
        static std::string make_message(TokenType type);
    };

}

#endif // INC_POLISHD_EXCEPTIONS_HPP
