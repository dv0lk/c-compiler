#pragma once
#include <string>

namespace compiler::x86 {
    enum class register_t {
        RAX,
        RBX,
        RCX,
        RDX,
        RSI,
        RDI,
        RBP,
        RSP,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15
    };

    struct Register {
        register_t type_;

        explicit Register(const register_t type) : type_(type) {}

        friend bool operator==(const Register& lhs, const Register& rhs) {
            return lhs.type_ == rhs.type_;
        }
    };


    struct PseudoRegister {
        std::string name;

        PseudoRegister() = default;

        explicit PseudoRegister(const std::string& name)
            : name(name) {}

        friend bool operator==(const PseudoRegister& lhs, const PseudoRegister& rhs) = default;
    };
}
