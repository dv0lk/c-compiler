#pragma once
#include <array>

#include "core/program.hpp"
#include "ir/types/instruction.hpp"
#include "ir/types/operand.hpp"
#include "types/instruction.hpp"

namespace compiler::x86 {
    // windows x64 calling convention register order
    inline constexpr std::array ARG_REGISTERS = {RegType::RCX, RegType::RDX, RegType::R8, RegType::R9};

    class Emitter {
    public:
        Program<Instruction> emit(const Program<ir::Instruction>& ir_program);

        [[nodiscard]] static Program<Instruction> get_x86(const Program<ir::Instruction>& ir_program) {
            Emitter assembler;
            return assembler.emit(ir_program);
        }

    private:
        Program<Instruction> x86_program_;
        Function<Instruction> current_function_;

        Operand convert_virt_reg(const ir::Operand& virtual_reg);

        void assemble_function(const Function<ir::Instruction>& function);

        void assemble(const ir::Return&);

        void assemble(const ir::Binary&);

        void assemble(const ir::Unary&);

        void assemble(const ir::Copy&);

        void assemble(const ir::Label&);

        void assemble(const ir::Jump&);

        void assemble(const ir::JumpIfZero&);

        void assemble(const ir::JumpIfNotZero&);

        void assemble(const ir::FunctionCall&);
    };

} // namespace compiler::x86
