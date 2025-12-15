cmake_minimum_required(VERSION 3.30)
project(compiler-rewrite)

set(CMAKE_CXX_STANDARD 23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(compiler-rewrite src/main.cpp
        src/util/files.h
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/lexer/Token.h
        src/parser/ast.cpp
        src/parser/ast.h
        src/ir/types/instruction.hpp
        src/ir/printer.h
        src/ir/generator.cpp
        src/ir/generator.h
        src/scope/resolver.hpp
        src/optimizations/optimizer.hpp
        src/optimizations/passes/constant_folding.hpp
        src/optimizations/passes/constant_folding.cpp
        src/analysis/cfg.hpp
        src/optimizations/passes/unreachable_code_elem.cpp
        src/optimizations/passes/unreachable_code_elem.hpp
        src/optimizations/passes/copy_propagation.hpp
        src/optimizations/passes/copy_propagation.cpp
        src/codegen/x86_instructions.hpp
        src/codegen/code_generator.cpp
        src/codegen/code_generator.hpp
        src/compiler/compiler.cpp
        src/compiler/compiler.hpp
        src/codegen/register.hpp
        src/ir/types/basic_block_t.hpp
        src/ir/types/basic_block_t.hpp
        src/ir/types/value_t.hpp
        src/parser/ast.hpp
        src/parser/ast/expr.hpp
        src/parser/ast/stmt.hpp
        src/ir/ir.hpp
        src/analysis/adapter/ir_adapter.hpp
        src/analysis/node_t.hpp
        src/transforms/manager.hpp
        src/transforms/transform.hpp
        src/transforms/transforms/constant_fold.hpp
        src/transforms/transforms/copy_propagation.hpp
        src/transforms/transforms/utils.hpp
        src/transforms/transforms/unreachable_code_elem.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/util
)
