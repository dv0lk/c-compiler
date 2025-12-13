#pragma once
#include "adapter_t.hpp"
#include "ir/types/basic_block_t.hpp"

namespace compiler::cfg {
    template<typename T>
    struct instruction_adapter;

    template<>
    struct instruction_adapter<ir::instruction> {

        bool is_block_starter(const ir::instruction& i) {
            return is_label(i);
        }

        bool is_block_terminator(const ir::instruction& i) {
            return is_unconditional_jump(i) || is_conditional_jump(i) || is_return(i);
        }

        bool is_label(const ir::instruction& instr) {
            return instr.holds<ir::label>();
        }

        bool is_conditional_jump(const ir::instruction& i) {
            return i.holds<ir::jump_if_zero>() || i.holds<ir::jump_if_not_zero>();
        }

        bool is_unconditional_jump(const ir::instruction& i) {
            return i.holds<ir::jump>();
        }

        bool is_return(const ir::instruction& i) {
            return i.holds<ir::return_>();
        }

        std::string get_label_name(const ir::instruction& instruction);

        std::string get_jump_label(const ir::instruction& instruction);
    };
}
