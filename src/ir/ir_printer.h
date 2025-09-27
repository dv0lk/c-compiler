#pragma once
#include <format>
#include <sstream>
#include <string>
#include <vector>

#include "ir/ir.h"

namespace compiler::ir::printer {
    class ir_printer {
    public:
        static std::string to_string(const std::vector<ir_basic_block>& blocks) {
            std::stringstream ss;
            for (const auto& block : blocks) {
                ss << block.get_name() << ":\n";
                ss << to_string(block.get_instructions());
                ss << "\n";
            }
            return ss.str();
        }


        static std::string to_string(const std::vector<ir_instruction>& instructions) {
            std::stringstream ss;
            for (const auto& instruction : instructions) {
                ss << to_string(instruction) << "\n";
            }
            return ss.str();
        }

        static std::string to_string(const ir_instruction& instruction) {
            return std::visit([](const auto& instr) -> std::string {
                return to_string(instr);
            }, instruction);
        }

    private:
        template <class... Ts>
        struct overload : Ts... {
            using Ts::operator()...;
        };

        static std::string value_to_string(const ir_value& value) {
            if (value.is_constant())
                return std::to_string(value.get<int>());

            return value.get<std::string>();
        }

        static std::string to_string(const ir_return& ret) {
            return std::format("return {}", value_to_string(ret.value));
        }

        static std::string to_string(const ir_binary& binary) {
            return std::format("{} = {} {} {}",
                               value_to_string(binary.result),
                               value_to_string(binary.left),
                               token_to_string(binary.op),
                               value_to_string(binary.right));
        }

        static std::string to_string(const ir_unary& unary) {
            return std::format("{} = {}{}",
                               value_to_string(unary.result),
                               token_to_string(unary.op),
                               value_to_string(unary.value));
        }

        static std::string to_string(const ir_copy& copy) {
            return std::format("{} = {}",
                               value_to_string(copy.destination),
                               value_to_string(copy.source));
        }

        static std::string to_string(const ir_label& label) {
            return std::format("{}:", label.name);
        }

        static std::string to_string(const ir_jump& jump) {
            return std::format("jump {}", jump.label.name);
        }

        static std::string to_string(const ir_jump_if_zero& jump) {
            return std::format("jump_if_zero {}, {}",
                               value_to_string(jump.condition),
                               jump.label.name);
        }

        static std::string to_string(const ir_jump_if_not_zero& jump) {
            return std::format("jump_if_not_zero {}, {}",
                               value_to_string(jump.condition),
                               jump.label.name);
        }

        static std::string to_string(const ir_call& call) {
            std::string args;
            for (size_t i = 0; i < call.arguments.size(); i++) {
                if (i > 0) {
                    args += ", ";
                }
                args += value_to_string(call.arguments[i]);
            }

            return std::format("{} = call {}( {} )", value_to_string(call.destination), call.function_name, args);
        }
        static std::string token_to_string(const token_type type) {
            switch (type) {
            case token_type::Plus:
                return "+";
            case token_type::Minus:
                return "-";
            case token_type::Star:
                return "*";
            case token_type::Slash:
                return "/";
            case token_type::LogicalAnd:
                return "&&";
            case token_type::LogicalOr:
                return "||";
            case token_type::EqualEqual:
                return "==";
            case token_type::NotEqual:
                return "!=";
            case token_type::Less:
                return "<";
            case token_type::LessEqual:
                return "<=";
            case token_type::Greater:
                return ">";
            case token_type::GreaterEqual:
                return ">=";
            case token_type::Not:
                return "!";
            case token_type::MinusMinus:
                return "--";
            case token_type::PlusPlus:
                return "++";
            default:
                return "?";
            }
        }
    };
}
