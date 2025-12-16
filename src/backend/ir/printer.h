// #pragma once
// #include <format>
// #include <sstream>
// #include <string>
// #include <vector>
//
// #include "ir.hpp"
//
// namespace compiler::ir::printer {
//     class printer {
//     public:
//         static std::string to_string(const program_t& program) {
//             std::stringstream ss;
//             for (const auto &func: program.functions) {
//                 ss << "Function: " << func.name << "\n";
//                 ss << "=====================\n\n";
//                 ss << to_string(func);
//                 ss << "=====================\n\n";
//             }
//             return ss.str();
//         }
//
//         static std::string to_string(const function_t& func) {
//             return to_string(func.blocks);
//         }
//
//         static std::string to_string(const std::vector<basic_block_t> &blocks) {
//             std::stringstream ss;
//             int count = 1;
//             for (const auto &block: blocks) {
//                 ss << "Block#" << count << "\n";
//                 ss << "=====================\n";
//                 // ss << block.name() << ":\n";
//                 ss << to_string(block.instructions());
//                 ss << "\n";
//                 ss << "=====================\n\n";
//                 count++;
//             }
//             return ss.str();
//         }
//
//
//         static std::string to_string(const std::span<const instruction> &instructions) {
//             std::stringstream ss;
//             for (const auto &instruction: instructions) {
//                 ss << " " << to_string(instruction) << "\n";
//             }
//             return ss.str();
//         }
//
//         static std::string to_string(const instruction &instruction) {
//             return instruction.visit([](const auto &instr) -> std::string {
//                 return to_string(instr);
//             });
//         }
//
//     private:
//         template<class... Ts>
//         struct overload : Ts... {
//             using Ts::operator()...;
//         };
//
//         static std::string value_to_string(const value_t &value) {
//             return value.to_string();
//         }
//
//         static std::string to_string(const return_ &ret) {
//             return std::format("return {}", value_to_string(ret.value));
//         }
//
//         static std::string to_string(const binary &binary) {
//             return std::format("{} = {} {} {}",
//                                value_to_string(binary.result),
//                                value_to_string(binary.left),
//                                token_to_string(binary.op),
//                                value_to_string(binary.right));
//         }
//
//         static std::string to_string(const unary &unary) {
//             return std::format("{} = {}{}",
//                                value_to_string(unary.result),
//                                token_to_string(unary.op),
//                                value_to_string(unary.value));
//         }
//
//         static std::string to_string(const copy &copy) {
//             return std::format("{} = {}",
//                                value_to_string(copy.destination),
//                                value_to_string(copy.source));
//         }
//
//         static std::string to_string(const label &label) {
//             return std::format("{}:", label.name);
//         }
//
//         static std::string to_string(const jump &jump) {
//             return std::format("jump {}", jump.target_label.name);
//         }
//
//         static std::string to_string(const jump_if_zero &jump) {
//             return std::format("jump_if_zero {}, {}",
//                                value_to_string(jump.condition),
//                                jump.target_label.name);
//         }
//
//         static std::string to_string(const jump_if_not_zero &jump) {
//             return std::format("jump_if_not_zero {}, {}",
//                                value_to_string(jump.condition),
//                                jump.target_label.name);
//         }
//
//         static std::string to_string(const func_call &call) {
//             std::string args;
//             for (size_t i = 0; i < call.arguments.size(); i++) {
//                 if (i > 0) {
//                     args += ", ";
//                 }
//                 args += value_to_string(call.arguments[i]);
//             }
//
//             return std::format("{} = call {}( {} )", value_to_string(call.destination), call.function_name, args);
//         }
//
//         static std::string token_to_string(const token_t type) {
//             switch (type) {
//                 case token_t::Plus:
//                     return "+";
//                 case token_t::Minus:
//                     return "-";
//                 case token_t::Star:
//                     return "*";
//                 case token_t::Slash:
//                     return "/";
//                 case token_t::LogicalAnd:
//                     return "&&";
//                 case token_t::LogicalOr:
//                     return "||";
//                 case token_t::EqualEqual:
//                     return "==";
//                 case token_t::NotEqual:
//                     return "!=";
//                 case token_t::Less:
//                     return "<";
//                 case token_t::LessEqual:
//                     return "<=";
//                 case token_t::Greater:
//                     return ">";
//                 case token_t::GreaterEqual:
//                     return ">=";
//                 case token_t::Not:
//                     return "!";
//                 case token_t::MinusMinus:
//                     return "--";
//                 case token_t::PlusPlus:
//                     return "++";
//                 default:
//                     return "?";
//             }
//         }
//     };
// }
