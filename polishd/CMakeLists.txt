file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(polishd Token.hpp Grammar.cpp Function.cpp Compiler.cpp)

target_include_directories(polishd PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(polishd PRIVATE -Wall -Wextra -Wpedantic)
