#pragma once
#include <stdexcept>

#include "ir/types/instruction.hpp"

namespace compiler {

    template<typename InstrType>
    struct InstructionTrait;

    template<>
    struct InstructionTrait<ir::Instruction> {
        static bool is_label(const ir::Instruction& instr) {
            return instr.holds<ir::Label>();
        }

        static bool is_unconditional_jump(const ir::Instruction &instr) {
            return instr.holds<ir::Jump>();
        }

        static bool is_conditional_jump(const ir::Instruction &instr) {
            return instr.holds<ir::JumpIfZero, ir::JumpIfNotZero>();
        }

        static bool is_return(const ir::Instruction &instr) {
            return instr.holds<ir::Return>();
        }

        static bool is_terminator(const ir::Instruction &instr) {
            return is_unconditional_jump(instr) || is_conditional_jump(instr) || is_return(instr);
        }

        static std::string get_jump_target(const ir::Instruction &instr) {
            if (auto *jmp = instr.get_if<ir::Jump>()) {
                return jmp->target.name;
            }
            if (auto *jmp = instr.get_if<ir::JumpIfZero>()) {
                return jmp->target_label.name;
            }
            if (auto *jmp = instr.get_if<ir::JumpIfNotZero>()) {
                return jmp->target_label.name;
            }

            throw std::runtime_error("Error");
        }

        static std::string get_label_name(const ir::Instruction &instr) {
            if (auto *label = instr.get_if<ir::Label>()) {
                return label->name;
            }
            throw std::runtime_error("Error");
        }


        // Liveness analysis - get variables defined by an instruction
        static std::vector<std::string> get_defs(const ir::Instruction& instr) {
            return instr.visit([](const auto& i) -> std::vector<std::string> {
                using T = std::decay_t<decltype(i)>;
                if constexpr (std::is_same_v<T, ir::Binary>) {
                    return {i.result.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::Unary>) {
                    return {i.result.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::Copy>) {
                    return {i.destination.get_variable()};
                } else if constexpr (std::is_same_v<T, ir::FunctionCall>) {
                    return {i.destination.get_variable()};
                } else {
                    return {};
                }
            });
        }

        static std::vector<std::string> get_uses(const ir::Instruction& instr) {
            return instr.visit([](const auto& i) -> std::vector<std::string> {
                using T = std::decay_t<decltype(i)>;
                std::vector<std::string> result;

                if constexpr (std::is_same_v<T, ir::Binary>) {
                    if (i.left.is_variable()) result.push_back(i.left.get_variable());
                    if (i.right.is_variable()) result.push_back(i.right.get_variable());
                } else if constexpr (std::is_same_v<T, ir::Unary>) {
                    if (i.value.is_variable()) result.push_back(i.value.get_variable());
                } else if constexpr (std::is_same_v<T, ir::Copy>) {
                    if (i.source.is_variable()) result.push_back(i.source.get_variable());
                } else if constexpr (std::is_same_v<T, ir::JumpIfZero>) {
                    if (i.condition.is_variable()) result.push_back(i.condition.get_variable());
                } else if constexpr (std::is_same_v<T, ir::JumpIfNotZero>) {
                    if (i.condition.is_variable()) result.push_back(i.condition.get_variable());
                } else if constexpr (std::is_same_v<T, ir::Return>) {
                    if (i.value.is_variable()) result.push_back(i.value.get_variable());
                } else if constexpr (std::is_same_v<T, ir::FunctionCall>) {
                    for (const auto& arg : i.arguments) {
                        if (arg.is_variable()) result.push_back(arg.get_variable());
                    }
                }
                return result;
            });
        }

        static bool has_side_effects(const ir::Instruction& instr) {
            return instr.holds<ir::FunctionCall>();
        }
    };
}
