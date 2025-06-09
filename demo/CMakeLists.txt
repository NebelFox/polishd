cmake_minimum_required(VERSION 3.19)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp REPL.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE polishd)

target_include_directories(${PROJECT_NAME} PRIVATE
	${PROJECT_SOURCE_DIR}/polishd
)
