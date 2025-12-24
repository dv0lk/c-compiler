#pragma once
#include "types/instruction.hpp"
#include "structure/program.hpp"
#include "structure/basic_block.hpp"
#include "ir/types/instruction.hpp"
#include "ir/types/operand.hpp"
namespace compiler::x86 {
    class emitter {
    public:
        Program<Instruction> emit(const Program<ir::Instruction> &ir_program);

        [[nodiscard]] static Program<Instruction> get_x86(const Program<ir::Instruction> &ir_program) {
            emitter assembler;
            return assembler.emit(ir_program);
        }

    private:
        Program<Instruction> x86_program_;
        Function<Instruction> current_function_;
        BasicBlock<Instruction> current_bb_;

        Operand convert_virt_reg(const ir::Operand& virtual_reg);

        void assemble_function(const Function<ir::Instruction>& function);

        void assemble_bb(const BasicBlock<ir::Instruction>& block);

        void assemble_instruction(const ir::Instruction&);

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

}
