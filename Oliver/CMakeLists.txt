# CMakeList.txt : CMake project for Olly, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Oliver "Oliver.cpp" "Olly.h" 
	"interpreter/let/let.h" 
	"interpreter/compiler/parser.h" 
	"interpreter/evaluator/evaluator.h"
	"interpreter/compiler/compiler.h" 
  "interpreter/evaluator/unary_operators.h" )

# TODO: Add tests and install targets if needed.
