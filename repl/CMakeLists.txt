add_executable(repl main.cpp Calculator.cpp)

target_link_libraries(repl PRIVATE polishd)

target_include_directories(repl PRIVATE
	${PROJECT_SOURCE_DIR}/polishd
)
