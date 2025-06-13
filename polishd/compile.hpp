#ifndef INC_POLISHD_COMPILE_HPP
#define INC_POLISHD_COMPILE_HPP

#include <string>

#include <Grammar.hpp>
#include <Function.hpp>

namespace polishd {

    Function compile(const Grammar& grammar, const std::string& infix);

}

#endif // INC_POLISHD_COMPILE_HPP
