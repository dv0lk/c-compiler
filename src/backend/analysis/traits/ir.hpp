#pragma once
#include <stdexcept>

#include "ir/types/instruction.hpp"

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
                return jmp->target.name;
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


        // Liveness analysis - get variables defined by an instruction
        static std::vector<std::string> get_defs(const ir::instruction& instr) {
            return instr.visit([](const auto& i) -> std::vector<std::string> {
                using T = std::decay_t<decltype(i)>;
                if constexpr (std::is_same_v<T, ir::binary>) {
                    return {i.result.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::unary>) {
                    return {i.result.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::copy>) {
                    return {i.destination.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::func_call>) {
                    return {i.destination.get_variable()};
                } else {
                    return {};
                }
            });
        }

        // Liveness analysis - get variables used by an instruction
        static std::vector<std::string> get_uses(const ir::instruction& instr) {
            return instr.visit([](const auto& i) -> std::vector<std::string> {
                using T = std::decay_t<decltype(i)>;
                std::vector<std::string> result;

                if constexpr (std::is_same_v<T, ir::binary>) {
                    if (i.left.is_variable()) result.push_back(i.left.get_variable());
                    if (i.right.is_variable()) result.push_back(i.right.get_variable());
                } else if constexpr (std::is_same_v<T, ir::unary>) {
                    if (i.value.is_variable()) result.push_back(i.value.get_variable());
                } else if constexpr (std::is_same_v<T, ir::copy>) {
                    if (i.source.is_variable()) result.push_back(i.source.get_variable());
                } else if constexpr (std::is_same_v<T, ir::jump_if_zero>) {
                    if (i.condition.is_variable()) result.push_back(i.condition.get_variable());
                } else if constexpr (std::is_same_v<T, ir::jump_if_not_zero>) {
                    if (i.condition.is_variable()) result.push_back(i.condition.get_variable());
                } else if constexpr (std::is_same_v<T, ir::return_>) {
                    if (i.value.is_variable()) result.push_back(i.value.get_variable());
                } else if constexpr (std::is_same_v<T, ir::func_call>) {
                    for (const auto& arg : i.arguments) {
                        if (arg.is_variable()) result.push_back(arg.get_variable());
                    }
                }
                return result;
            });
        }
    };
}
