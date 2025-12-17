#include <stdexcept>
#include <string>

#include "x86/types/instruction.hpp"


namespace compiler {
    template<typename InstrType>
    struct InstructionTrait;

    template<>
    struct InstructionTrait<x86::Instruction> {
        static bool is_label(const x86::Instruction &instr) {
            return instr.holds<x86::Label>();
        }

        static bool is_unconditional_jump(const x86::Instruction &instr) {
            return instr.holds<x86::Jmp>();
        }

        static bool is_conditional_jump(const x86::Instruction &instr) {
            return instr.holds<x86::JmpCC>();
        }

        static bool is_return(const x86::Instruction &instr) {
            return instr.holds<x86::Ret>();
        }

        static bool is_terminator(const x86::Instruction &instr) {
            return is_unconditional_jump(instr) || is_conditional_jump(instr) || is_return(instr);
        }

        static std::string get_jump_target(const x86::Instruction &instr) {
            if (auto *jmp = instr.get_if<x86::Jmp>()) {
                return jmp->target.name;
            }

            throw std::runtime_error("Error");
        }

        static std::string get_label_name(const x86::Instruction &instr) {
            if (auto *label = instr.get_if<x86::Label>()) {
                return label->target.name;
            }
            throw std::runtime_error("Error");
        }
    };
}
