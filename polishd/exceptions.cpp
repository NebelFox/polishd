#include <exceptions.hpp>

#include <sstream>
#include <string_view>

namespace polishd
{

    Exception::Exception(const std::string& what) : std::runtime_error(what) {}

    MissingArgumentError::MissingArgumentError(const std::string& arg_name) : Exception("Missing argument: " + arg_name) {}

    ExpressionSyntaxError::ExpressionSyntaxError(const std::string& what) : Exception("Invalid expression syntax: " + what) {}

    namespace
    {
        
        std::string_view token_type_to_string(TokenType type)
        {
            switch(type) {
                case TokenType::None:
                    return "None";
                case TokenType::Argument:
                    return "Argument";
                case TokenType::Number:
                    return "Number";
                case TokenType::Prefix:
                    return "Prefix";
                case TokenType::Binary:
                    return "Binary";
                case TokenType::Postfix:
                    return "Postfix";
                case TokenType::Opening:
                    return "Opening";
                case TokenType::Closing:
                    return "Closing";
            }
        }

    }
    
    UnexpectedTokenError::UnexpectedTokenError(const Token& token) : Exception(make_message(token)) {}
    
    std::string UnexpectedTokenError::make_message(const Token& token)
    {
        std::stringstream s;
        s << "Unexpected token: Token{.type = " << token_type_to_string(token.type) << ", .value = '" << token.value << "'}";
        return s.str();
    }
    
    UnexpectedUnitError::UnexpectedUnitError(TokenType type) : Exception(make_message(type)) {}

    std::string UnexpectedUnitError::make_message(TokenType type)
    {
        std::stringstream s;
        s << "Unexpected unit: TokenType is " << token_type_to_string(type);
        return s.str();
    }

} // namespace polishd
