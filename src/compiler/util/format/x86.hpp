#pragma once
#include "x86/types/instruction.hpp"
#include <format>

template <>
struct std::formatter<compiler::x86::RegType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(compiler::x86::RegType reg, std::format_context& ctx) const {
        using enum compiler::x86::RegType;
        const char* name = [reg] {
            switch (reg) {
            case RAX:
                return "rax";
            case RBX:
                return "rbx";
            case RCX:
                return "rcx";
            case RDX:
                return "rdx";
            case RSI:
                return "rsi";
            case RDI:
                return "rdi";
            case RBP:
                return "rbp";
            case RSP:
                return "rsp";
            case R8:
                return "r8";
            case R9:
                return "r9";
            case R10:
                return "r10";
            case R11:
                return "r11";
            case R12:
                return "r12";
            case R13:
                return "r13";
            case R14:
                return "r14";
            case R15:
                return "r15";
            }
            return "???";
        }();
        return std::format_to(ctx.out(), "{}", name);
    }
};

template <>
struct std::formatter<compiler::x86::CC> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(compiler::x86::CC cc, std::format_context& ctx) const {
        using enum compiler::x86::CC;
        const char* suffix = [cc] {
            switch (cc) {
            case Empty:
                return "";
            case Equal:
                return "e";
            case NotEqual:
                return "ne";
            case Less:
                return "l";
            case LessOrEqual:
                return "le";
            case Greater:
                return "g";
            case GreaterOrEqual:
                return "ge";
            case Below:
                return "b";
            case BelowOrEqual:
                return "be";
            case Above:
                return "a";
            case AboveOrEqual:
                return "ae";
            case Overflow:
                return "o";
            case NoOverflow:
                return "no";
            case Sign:
                return "s";
            case NotSign:
                return "ns";
            case ParityEven:
                return "p";
            case ParityOdd:
                return "np";
            }
            return "?";
        }();
        return std::format_to(ctx.out(), "{}", suffix);
    }
};

template <>
struct std::formatter<compiler::x86::Imm> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Imm& imm, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", imm.value);
    }
};

template <>
struct std::formatter<compiler::x86::LabelOp> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::LabelOp& label, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", label.name);
    }
};

template <>
struct std::formatter<compiler::x86::Mem> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Mem& mem, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "qword [rbp{:+}]", mem.offset);
    }
};

template <>
struct std::formatter<compiler::x86::Register> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Register& reg, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", reg.type_);
    }
};

template <>
struct std::formatter<compiler::x86::PseudoRegister> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::PseudoRegister& reg, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", reg.name);
    }
};

template <>
struct std::formatter<compiler::x86::Operand> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Operand& op, std::format_context& ctx) const {
        return op.visit([&ctx](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return std::format_to(ctx.out(), "<empty>");
            } else {
                return std::format_to(ctx.out(), "{}", v);
            }
        });
    }
};

template <>
struct std::formatter<compiler::x86::Mov> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Mov& mov, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "mov {}, {}", mov.destination, mov.source);
    }
};

template <>
struct std::formatter<compiler::x86::Ret> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Ret&, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "ret");
    }
};

template <>
struct std::formatter<compiler::x86::Neg> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Neg& neg, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "neg {}", neg.value);
    }
};

template <>
struct std::formatter<compiler::x86::Not> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Not& n, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "not {}", n.value);
    }
};

template <>
struct std::formatter<compiler::x86::Add> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Add& add, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "add {}, {}", add.destination, add.source);
    }
};

template <>
struct std::formatter<compiler::x86::Sub> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Sub& sub, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "sub {}, {}", sub.destination, sub.source);
    }
};

template <>
struct std::formatter<compiler::x86::Imul> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Imul& imul, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "imul {}, {}", imul.destination, imul.source);
    }
};

template <>
struct std::formatter<compiler::x86::Cdq> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Cdq&, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "cqo");
    }
};

template <>
struct std::formatter<compiler::x86::Idiv> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Idiv& idiv, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "idiv {}", idiv.value);
    }
};

template <>
struct std::formatter<compiler::x86::Cmp> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Cmp& cmp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "cmp {}, {}", cmp.destination, cmp.source);
    }
};

template <>
struct std::formatter<compiler::x86::Label> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Label& label, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}:", label.target);
    }
};

template <>
struct std::formatter<compiler::x86::Jmp> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Jmp& jmp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "jmp {}", jmp.target);
    }
};

template <>
struct std::formatter<compiler::x86::JmpCC> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::JmpCC& jcc, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "j{} {}", jcc.condition, jcc.target);
    }
};

template <>
struct std::formatter<compiler::x86::SetCC> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::SetCC& setcc, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "set{} {}", setcc.condition, setcc.value);
    }
};

template <>
struct std::formatter<compiler::x86::Push> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Push& push, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "push {}", push.value);
    }
};

template <>
struct std::formatter<compiler::x86::Pop> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Pop& pop, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "pop {}", pop.value);
    }
};

template <>
struct std::formatter<compiler::x86::Call> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Call& call, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "call {}", call.target);
    }
};

template <>
struct std::formatter<compiler::x86::Instruction> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::x86::Instruction& instr, std::format_context& ctx) const {
        return instr.visit([&ctx](const auto& op) {
            using T = std::decay_t<decltype(op)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return std::format_to(ctx.out(), "<empty>");
            } else {
                return std::format_to(ctx.out(), "{}", op);
            }
        });
    }
};

template <>
struct std::formatter<compiler::BasicBlock<compiler::x86::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::BasicBlock<compiler::x86::Instruction>& block, std::format_context& ctx) const {
        auto out = ctx.out();
        for (const auto& instr : block.instructions()) {
            out = std::format_to(out, "    {}\n", instr);
        }
        return out;
    }
};

template <>
struct std::formatter<compiler::Function<compiler::x86::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::Function<compiler::x86::Instruction>& fn, std::format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "fn {}(", fn.name());

        auto params = fn.params();
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0)
                out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", params[i]);
        }
        out = std::format_to(out, "):\n");

        for (const auto& instr : fn.instructions()) {
            out = std::format_to(out, "    {}\n", instr);
        }
        return out;
    }
};

template <>
struct std::formatter<compiler::Program<compiler::x86::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const compiler::Program<compiler::x86::Instruction>& prog, std::format_context& ctx) const {
        auto out = ctx.out();
        for (const auto& fn : prog.functions()) {
            out = std::format_to(out, "{}\n", fn);
        }
        return out;
    }
};