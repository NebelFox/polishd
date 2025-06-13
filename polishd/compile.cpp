#include <compile.hpp>

#include <CompilingContext.hpp>

namespace polishd {

    Function polishd::compile(const Grammar& grammar, const std::string& infix)
    {
        return CompilingContext(grammar, infix).compile();
    }

}
