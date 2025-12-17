#pragma once
#include "types/instruction.hpp"
#include "base/base.hpp"
#include "ir/types/instruction.hpp"
#include "ir/types/virt_reg.hpp"
namespace compiler::x86 {
    class emitter {
    public:
        Program<Instruction> emit(const Program<ir::instruction> &ir_program);

        [[nodiscard]] static Program<Instruction> get_x86(const Program<ir::instruction> &ir_program) {
            emitter assembler;
            return assembler.emit(ir_program);
        }

    private:
        Program<Instruction> x86_program_;
        Function<Instruction> current_function_;
        BasicBlock<Instruction> current_bb_;

        x86::Operand convert_virt_reg(const ir::VirtualReg& virtual_reg);

        void assemble_function(const Function<ir::instruction>& function);

        void assemble_bb(const BasicBlock<ir::instruction>& block);

        void assemble_instruction(const ir::instruction& instr);

        void assemble(const ir::return_ &ret);

        void assemble(const ir::binary &binary);

        void assemble(const ir::unary &unary);

        void assemble(const ir::copy &copy);

        void assemble(const ir::label &label);

        void assemble(const ir::jump &jump);

        void assemble(const ir::jump_if_zero &jump_if_zero);

        void assemble(const ir::jump_if_not_zero &jump_if_not_zero);

        void assemble(const ir::func_call &func_call);
    };

}
