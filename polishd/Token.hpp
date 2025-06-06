#ifndef INC_POLISHD_TOKEN_HPP
#define INC_POLISHD_TOKEN_HPP

#include <string>
#include <stack>
#include <functional>
#include <map>

namespace polishd {

    using Stack = std::stack<double>&;
    using Args = const std::map<std::string, double>&;
    using Unit = std::function<double (Stack, Args)>;


    enum class TokenType : unsigned char
    {
        None = 0,
        Number,
        Prefix,
        Binary,
        Postfix,
        Opening,
        Closing,
        Argument
    };


    struct Token
    {
        TokenType type;
        std::string value;
    };

} // namespace polishd

#endif //INC_POLISHD_TOKEN_HPP
