#ifndef INC_POLISHD_TOKEN_HPP
#define INC_POLISHD_TOKEN_HPP

#include <string_view>

namespace polishd {

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
        std::string_view value;
    };

} // namespace polishd

#endif //INC_POLISHD_TOKEN_HPP
