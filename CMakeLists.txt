cmake_minimum_required(VERSION 3.30)
project(compiler-rewrite)

set(CMAKE_CXX_STANDARD 23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(compiler-rewrite src/main.cpp
        src/util/files.h
        src/frontend/lexer/tokenizer.cpp
        src/frontend/lexer/tokenizer.h
        src/frontend/lexer/Token.h
        src/frontend/parser/ast.cpp
        src/frontend/parser/ast.hpp
        src/backend/ir/types/instruction.hpp
        src/util/printer.h
        src/backend/ir/ir_emitter.cpp
        src/backend/ir/ir_emitter.h
        src/backend/ir/util/resolver.hpp
        src/backend/analysis/CFG.hpp
        src/backend/codegen/x86_instructions.hpp
        src/backend/codegen/code_generator.cpp
        src/backend/codegen/code_generator.hpp
        src/backend/codegen/register.hpp
        src/backend/ir/types/virt_reg.hpp
        src/frontend/parser/ast/expr.hpp
        src/frontend/parser/ast/stmt.hpp
        src/backend/ir/types/ir.hpp
        src/backend/analysis/Node.hpp
        src/backend/transforms/manager.hpp
        src/backend/transforms/transform.hpp
        src/backend/transforms/transforms/constant_fold.hpp
        src/backend/transforms/transforms/copy_propagation.hpp
        src/backend/transforms/transforms/utils.hpp
        src/backend/transforms/transforms/unreachable_code_elem.hpp
        src/backend/base/program.hpp
        src/backend/analysis/traits/traits.hpp
        src/backend/base/basic_block.hpp
        src/backend/base/function.hpp
        src/backend/ir/types/types.hpp
        src/backend/x86/types/instruction.hpp
        src/backend/x86/types/operand/register.hpp
        src/backend/x86/types/operand/operand.hpp
        src/backend/x86/util/util.hpp
        src/backend/x86/types/operand/label.hpp
        src/backend/x86/types/operand/mem.hpp
        src/backend/x86/types/operand/imm.hpp
        src/backend/x86/types/mnemonic.hpp
        src/backend/x86/types/condition_code.hpp
        src/backend/x86/x86_emitter.hpp
        src/backend/base/base.hpp
        src/backend/x86/x86_emitter.cpp
        src/util/x86_printer.hpp
        src/backend/analysis/traits/ir.hpp
        src/backend/analysis/traits/x86.hpp
        src/backend/analysis/liveness.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/util
        ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
)
