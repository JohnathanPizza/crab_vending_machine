cmake_minimum_required(VERSION 3.19)

project(crab)

include_directories(${CMAKE_SOURCE_DIR}/inc)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(crab
	${CMAKE_SOURCE_DIR}/src/darray.c
	${CMAKE_SOURCE_DIR}/src/error.c
	${CMAKE_SOURCE_DIR}/src/expression.c
	${CMAKE_SOURCE_DIR}/src/label.c
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/src/opcode.c
	${CMAKE_SOURCE_DIR}/src/position.c
	${CMAKE_SOURCE_DIR}/src/utility.c
	${CMAKE_SOURCE_DIR}/src/word.c
)
