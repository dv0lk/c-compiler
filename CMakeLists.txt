cmake_minimum_required(VERSION 3.30)
project(compiler)

set(CMAKE_CXX_STANDARD 23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(compiler src/main.cpp
        src/util/files.h
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/lexer/Token.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/parser/AST.h
        src/ir/ir.h
        src/Ir/ir_printer.h
        src/ir/ir_generator.cpp
        src/ir/ir_generator.h
        src/scope/resolver.hpp
        src/optimizations/optimizer.hpp
        src/optimizations/passes/constant_folding.hpp
        src/optimizations/passes/constant_folding.cpp
        src/flow_graph/flow_graph.hpp
        src/optimizations/passes/unreachable_code_elem.cpp
        src/optimizations/passes/unreachable_code_elem.hpp
        src/optimizations/passes/copy_propagation.hpp
        src/optimizations/passes/copy_propagation.cpp
        src/codegen/x86_instructions.hpp
        src/codegen/code_generator.cpp
        src/codegen/code_generator.hpp
        src/compiler/compiler.cpp
        src/compiler/compiler.hpp
        src/codegen/register.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/util
)
