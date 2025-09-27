#pragma once
#include <vector>
#include "x86_instructions.hpp"
#include "ir/ir.h"

namespace compiler {
    class CodeGenerator {
    private:
        std::vector<x86::instruction> instructions;
        std::unordered_map<std::string, int> temp_var_locations;
        int current_stack_offset = -4;

    public:
        void generate(const std::vector<ir::ir_instruction>& ir_instructions);

        void add_instruction(const x86::instruction& instruction) {
            instructions.emplace_back(instruction);
        }

        x86::Operand convert_value(const ir::ir_value& value);

        int get_temp_location(const std::string& name);

        void assemble(const ir::ir_binary& binary);

        void assemble(const ir::ir_return& ret);

        void assemble(const ir::ir_unary& unary);

        void assemble(const ir::ir_copy& copy);

        void assemble(const ir::ir_label& label);

        void assemble(const ir::ir_jump& jump);

        void assemble(const ir::ir_jump_if_zero& jump);

        void assemble(const ir::ir_jump_if_not_zero& jump);

        void assemble(const ir::ir_call& call);

    };
}

