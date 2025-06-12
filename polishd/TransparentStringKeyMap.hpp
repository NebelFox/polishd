#ifndef INC_POLISHD_TRANSPARENT_STRING_KEY_MAP_HPP
#define INC_POLISHD_TRANSPARENT_STRING_KEY_MAP_HPP

#include <string>
#include <string_view>
#include <unordered_map>

namespace polishd {
    
    template <typename ...Ts>
    struct overload : Ts ...
    {
        using is_transparent = void;
        using Ts::operator() ...;
    };
    
    using transparentStringHash = overload<
        std::hash<std::string>,
        std::hash<std::string_view>
    >;

    template<typename ValueType>
    using TransparentStringKeyMap = std::unordered_map<std::string, ValueType, transparentStringHash, std::equal_to<>>;

} // namespace polishd


#endif // INC_POLISHD_TRANSPARENT_STRING_KEY_MAP_HPP
