#pragma once
#include <stdexcept>


#include "ir/types/basic_block_t.hpp"

namespace compiler::cfg {
    template<typename T>
    struct instruction_adapter;

    template<>
    struct instruction_adapter<ir::instruction> {

        [[nodiscard]] bool is_block_starter(const ir::instruction& i) const {
            return is_label(i);
        }

        [[nodiscard]]bool is_block_terminator(const ir::instruction& i) const {
            return is_unconditional_jump(i) || is_conditional_jump(i) || is_return(i);
        }

        [[nodiscard]] bool is_label(const ir::instruction& instr) const {
            return instr.holds<ir::label>();
        }

        [[nodiscard]] bool is_conditional_jump(const ir::instruction& i) const {
            return i.holds<ir::jump_if_zero>() || i.holds<ir::jump_if_not_zero>();
        }

        [[nodiscard]] bool is_unconditional_jump(const ir::instruction& i) const {
            return i.holds<ir::jump>();
        }

        [[nodiscard]] bool is_return(const ir::instruction& i) const {
            return i.holds<ir::return_>();
        }

        std::string get_label_name(const ir::instruction& instruction) {
            if (const auto instr = instruction.get_if<ir::label>()) {
                return instr->name;
            }
            throw std::runtime_error("Calling get_label_name on a non label instruction");
        }

        std::string get_jump_label(const ir::instruction& instruction) {
            if (const auto jmp = instruction.get_if<ir::jump>()) {
                return jmp->target_label.name;
            }

            if (const auto jmp = instruction.get_if<ir::jump_if_zero>()) {
                return jmp->target_label.name;
            }

            if (const auto jmp = instruction.get_if<ir::jump_if_not_zero>()) {
                return jmp->target_label.name;
            }

            throw std::runtime_error("Calling get_jump_label on non jump instruction");
        }
    };
}
