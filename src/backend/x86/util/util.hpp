#pragma once
#include <stdexcept>
#include <string>

#include "x86/types/condition_code.hpp"
#include "../types/operand/register.hpp"

namespace compiler::x86::util {
    [[nodiscard]] static std::string register_to_string(const register_t type) {
        switch (type) {
            default: throw std::runtime_error("Unknown register type");
            case register_t::RAX: return "rax";
            case register_t::RBX: return "rbx";
            case register_t::RCX: return "rcx";
            case register_t::RDX: return "rdx";
            case register_t::RDI: return "rdi";
            case register_t::RSI: return "rsi";
            case register_t::R8: return "r8";
            case register_t::R9: return "r9";
            case register_t::R12: return "r12";
            case register_t::R13: return "r13";
            case register_t::R14: return "r14";
            case register_t::R15: return "r15";
        }
    }

    [[nodiscard]] inline std::string cc_to_str(const CC cc) {
        switch (cc) {
            default: throw std::runtime_error("Unknown condition code");
            case CC::Equal:
                return "e";
            case CC::NotEqual:
                return "ne";
            case CC::Less:
                return "l";
            case CC::LessOrEqual:
                return "le";
            case CC::Greater:
                return "g";
            case CC::GreaterOrEqual:
                return "ge";
            case CC::Below:
                return "b";
            case CC::BelowOrEqual:
                return "be";
            case CC::Above:
                return "a";
            case CC::AboveOrEqual:
                return "ae";
            case CC::Overflow:
                return "o";
            case CC::NoOverflow:
                return "no";
            case CC::Sign:
                return "s";
            case CC::NotSign:
                return "ns";
            case CC::ParityEven:
                return "p";
            case CC::ParityOdd:
                return "np";
        }
    }
}
