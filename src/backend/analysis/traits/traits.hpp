#pragma once
#include "../../ir/types/ir.hpp"

namespace compiler {

    template<typename InstrType>
    struct InstructionTrait;

    template<>
    struct InstructionTrait<ir::instruction> {
        static bool is_label(const ir::instruction& instr) {
            return instr.holds<ir::label>();
        }

        static bool is_unconditional_jump(const ir::instruction &instr) {
            return instr.holds<ir::jump>();
        }

        static bool is_conditional_jump(const ir::instruction &instr) {
            return instr.holds<ir::jump_if_zero, ir::jump_if_not_zero>();
        }

        static bool is_return(const ir::instruction &instr) {
            return instr.holds<ir::return_>();
        }

        static bool is_terminator(const ir::instruction &instr) {
            return is_unconditional_jump(instr) || is_conditional_jump(instr) || is_return(instr);
        }

        static std::string get_jump_target(const ir::instruction &instr) {
            if (auto *jmp = instr.get_if<ir::jump>()) {
                return jmp->target_label.name;
            }
            if (auto *jmp = instr.get_if<ir::jump_if_zero>()) {
                return jmp->target_label.name;
            }
            if (auto *jmp = instr.get_if<ir::jump_if_not_zero>()) {
                return jmp->target_label.name;
            }

            throw std::runtime_error("Error");
        }

        static std::string get_label_name(const ir::instruction &instr) {
            if (auto *label = instr.get_if<ir::label>()) {
                return label->name;
            }
            throw std::runtime_error("Error");
        }
    };
}
