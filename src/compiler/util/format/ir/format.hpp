#pragma once
#include <format>
#include <print>
#include "ir/ir.hpp"
// #include "util.hpp"
#include "lexer/token.hpp"

template<>
struct std::formatter<compiler::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::TokenType& token, std::format_context& ctx) const {
        using enum compiler::TokenType;
        const char* str = [token] {
            switch (token) {
                case Plus:         return "+";
                case Minus:        return "-";
                case Star:         return "*";
                case Slash:        return "/";
                case LogicalAnd:   return "&&";
                case LogicalOr:    return "||";
                case EqualEqual:   return "==";
                case NotEqual:     return "!=";
                case Less:         return "<";
                case LessEqual:    return "<=";
                case Greater:      return ">";
                case GreaterEqual: return ">=";
                case Not:          return "!";
                case MinusMinus:   return "--";
                case PlusPlus:     return "++";
                default:           return "?";
            }
        }();
        return std::format_to(ctx.out(), "{}", str);
    }
};

template<>
struct std::formatter<compiler::ir::Operand> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Operand& op, std::format_context& ctx) const {
        if (op.is_constant()) {
            return std::format_to(ctx.out(), "{}", op.get_constant());
        }
        return std::format_to(ctx.out(), "{}", op.get_variable());
    }
};

// Label
template<>
struct std::formatter<compiler::ir::Label> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Label& label, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", label.name);
    }
};

// Return
template<>
struct std::formatter<compiler::ir::Return> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Return& ret, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "return {}", ret.value);
    }
};

// Binary
template<>
struct std::formatter<compiler::ir::Binary> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Binary& bin, std::format_context& ctx) const {
        // Adjust based on how you want to format token_t
        return std::format_to(ctx.out(), "{} = {} {} {}",
            bin.result, bin.left, bin.op, bin.right);
    }
};

template<>
struct std::formatter<compiler::ir::Unary> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Unary& un, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{} = {}{}",
            un.result, un.op, un.value);
    }
};

template<>
struct std::formatter<compiler::ir::Copy> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Copy& cp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{} = {}", cp.destination, cp.source);
    }
};

template<>
struct std::formatter<compiler::ir::Jump> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Jump& jmp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "jmp {}", jmp.target);
    }
};

template<>
struct std::formatter<compiler::ir::JumpIfZero> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::JumpIfZero& jz, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "jz {}, {}", jz.condition, jz.target_label);
    }
};

template<>
struct std::formatter<compiler::ir::JumpIfNotZero> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::JumpIfNotZero& jnz, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "jnz {}, {}", jnz.condition, jnz.target_label);
    }
};

template<>
struct std::formatter<compiler::ir::FunctionCall> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::FunctionCall& call, std::format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "{} = call {}(",
            call.destination, call.function_name);

        for (size_t i = 0; i < call.arguments.size(); ++i) {
            if (i > 0) out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", call.arguments[i]);
        }
        return std::format_to(out, ")");
    }
};

template<>
struct std::formatter<compiler::ir::Instruction> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::ir::Instruction& instr, std::format_context& ctx) const {
        return instr.visit([&ctx](const auto& op) {
            return std::format_to(ctx.out(), "{}", op);
        });
    }
};

template<>
struct std::formatter<compiler::BasicBlock<compiler::ir::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::BasicBlock<compiler::ir::Instruction>& block, std::format_context& ctx) const {
        auto out = ctx.out();
        for (const auto& instr : block.instructions()) {
            out = std::format_to(out, "    {}\n", instr);
        }
        return out;
    }
};

template<>
struct std::formatter<compiler::Function<compiler::ir::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::Function<compiler::ir::Instruction>& fn, std::format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "fn {}(", fn.name());

        auto params = fn.params();
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", params[i]);
        }
        out = std::format_to(out, "):\n");

        for (const auto& block : fn.basic_blocks()) {
            out = std::format_to(out, "{}", block);
        }
        return out;
    }
};

template<>
struct std::formatter<compiler::Program<compiler::ir::Instruction>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const compiler::Program<compiler::ir::Instruction>& prog, std::format_context& ctx) const {
        auto out = ctx.out();
        for (const auto& fn : prog.functions()) {
            out = std::format_to(out, "{}\n", fn);
        }
        return out;
    }
};