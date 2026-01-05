#pragma once
#include <stdexcept>
#include <string>
#include <vector>

#include "x86/types/instruction.hpp"

namespace compiler {
    template <typename InstrType>
    struct InstructionTrait;

    template <>
    struct InstructionTrait<x86::Instruction> {
        static bool is_label(const x86::Instruction& instr) {
            return instr.holds<x86::Label>();
        }

        static bool is_unconditional_jump(const x86::Instruction& instr) {
            return instr.holds<x86::Jmp>();
        }

        static bool is_conditional_jump(const x86::Instruction& instr) {
            return instr.holds<x86::JmpCC>();
        }

        static bool is_return(const x86::Instruction& instr) {
            return instr.holds<x86::Ret>();
        }

        static bool is_terminator(const x86::Instruction& instr) {
            return is_unconditional_jump(instr) || is_conditional_jump(instr) || is_return(instr);
        }

        static std::string get_jump_target(const x86::Instruction& instr) {
            if (auto* jmp = instr.get_if<x86::Jmp>()) {
                return jmp->target.name;
            }
            if (auto* jcc = instr.get_if<x86::JmpCC>()) {
                return jcc->target.name;
            }
            throw std::runtime_error("Error: not a jump instruction");
        }

        static std::string get_label_name(const x86::Instruction& instr) {
            if (auto* label = instr.get_if<x86::Label>()) {
                return label->target.name;
            }
            throw std::runtime_error("Error: not a label instruction");
        }

    private:
        static void add_if_pseudo(std::vector<std::string>& result, const x86::Operand& op) {
            if (auto* pseudo = op.get_if<x86::PseudoRegister>()) {
                result.push_back(pseudo->name);
            }
        }

    public:
        static std::vector<std::string> get_defs(const x86::Instruction& instr) {
            std::vector<std::string> result;

            instr.visit([&result](const auto& i) {
                using T = std::decay_t<decltype(i)>;

                if constexpr (std::is_same_v<T, x86::Mov>) {
                    add_if_pseudo(result, i.destination);
                } else if constexpr (std::is_same_v<T, x86::Add> || std::is_same_v<T, x86::Sub> || std::is_same_v<T, x86::Imul>) {
                    add_if_pseudo(result, i.destination);
                } else if constexpr (std::is_same_v<T, x86::Neg> || std::is_same_v<T, x86::Not>) {
                    add_if_pseudo(result, i.value);
                } else if constexpr (std::is_same_v<T, x86::SetCC>) {
                    add_if_pseudo(result, i.value);
                } else if constexpr (std::is_same_v<T, x86::Pop>) {
                    add_if_pseudo(result, i.value);
                }
            });

            return result;
        }

        static std::vector<std::string> get_uses(const x86::Instruction& instr) {
            std::vector<std::string> result;

            instr.visit([&result](const auto& i) {
                using T = std::decay_t<decltype(i)>;

                if constexpr (std::is_same_v<T, x86::Mov>) {
                    add_if_pseudo(result, i.source);
                } else if constexpr (std::is_same_v<T, x86::Add> || std::is_same_v<T, x86::Sub> || std::is_same_v<T, x86::Imul>) {
                    add_if_pseudo(result, i.destination);
                    add_if_pseudo(result, i.source);
                } else if constexpr (std::is_same_v<T, x86::Neg> || std::is_same_v<T, x86::Not>) {
                    add_if_pseudo(result, i.value);
                } else if constexpr (std::is_same_v<T, x86::Cmp>) {
                    add_if_pseudo(result, i.destination);
                    add_if_pseudo(result, i.source);
                } else if constexpr (std::is_same_v<T, x86::Idiv>) {
                    add_if_pseudo(result, i.value);
                } else if constexpr (std::is_same_v<T, x86::Push>) {
                    add_if_pseudo(result, i.value);
                } else if constexpr (std::is_same_v<T, x86::Call>) {
                    add_if_pseudo(result, i.target);
                }
            });

            return result;
        }
    };
} // namespace compiler
