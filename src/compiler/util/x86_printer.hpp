//
// #pragma once
//
// #include <iostream>
// #include <string>
// #include <string_view>
// #include <vector>
//
// #include "x86/types/instruction.hpp"
// #include "x86/util/util.hpp"
//
// namespace compiler::x86::util {
// namespace detail {
// template<typename>
// inline constexpr bool always_false_v = false;
//
// inline std::string operand_to_string(const Operand &operand) {
//     return operand.visit([](const auto &value) -> std::string {
//         using T = std::decay_t<decltype(value)>;
//         if constexpr (std::is_same_v<T, std::monostate>) {
//             return "-";
//         } else if constexpr (std::is_same_v<T, Register>) {
//             return register_to_string(value.type_);
//         } else if constexpr (std::is_same_v<T, Imm>) {
//             return std::to_string(value.value);
//         } else if constexpr (std::is_same_v<T, LabelOperand>) {
//             return value.name;
//         } else if constexpr (std::is_same_v<T, Mem>) {
//             return "[mem " + std::to_string(value.offset) + "]";
//         } else {
//             static_assert(always_false_v<T>, "Unsupported operand");
//         }
//     });
// }
//
// inline std::string format_binary(std::string_view mnemonic, const Operand &dst, const Operand &src) {
//     return std::string(mnemonic) + " " + operand_to_string(dst) + ", " + operand_to_string(src);
// }
//
// inline std::string format_unary(std::string_view mnemonic, const Operand &value) {
//     return std::string(mnemonic) + " " + operand_to_string(value);
// }
//
// template<typename T>
// std::string instruction_to_string_impl(const T &node) {
//     using value_t = std::decay_t<T>;
//     if constexpr (std::is_same_v<value_t, std::monostate>) {
//         return "nop";
//     } else if constexpr (std::is_same_v<value_t, Mov>) {
//         return format_binary("mov", node.destination, node.source);
//     } else if constexpr (std::is_same_v<value_t, Add>) {
//         return format_binary("add", node.destination, node.source);
//     } else if constexpr (std::is_same_v<value_t, Sub>) {
//         return format_binary("sub", node.destination, node.source);
//     } else if constexpr (std::is_same_v<value_t, Imul>) {
//         return format_binary("imul", node.destination, node.source);
//     } else if constexpr (std::is_same_v<value_t, Idiv>) {
//         return format_unary("idiv", node.value);
//     } else if constexpr (std::is_same_v<value_t, Neg>) {
//         return format_unary("neg", node.value);
//     } else if constexpr (std::is_same_v<value_t, Not>) {
//         return format_unary("not", node.value);
//     } else if constexpr (std::is_same_v<value_t, Push>) {
//         return format_unary("push", node.value);
//     } else if constexpr (std::is_same_v<value_t, Pop>) {
//         return format_unary("pop", node.value);
//     } else if constexpr (std::is_same_v<value_t, Cmp>) {
//         return format_binary("cmp", node.destination, node.source);
//     } else if constexpr (std::is_same_v<value_t, SetCC>) {
//         return "set" + cc_to_str(node.condition) + " " + operand_to_string(node.value);
//     } else if constexpr (std::is_same_v<value_t, Jmp>) {
//         return "jmp " + node.target.name;
//     } else if constexpr (std::is_same_v<value_t, JmpCC>) {
//         return "j" + cc_to_str(node.condition) + " " + node.target.name;
//     } else if constexpr (std::is_same_v<value_t, Label>) {
//         return node.target.name + ":";
//     } else if constexpr (std::is_same_v<value_t, Call>) {
//         return "call " + operand_to_string(node.target);
//     } else if constexpr (std::is_same_v<value_t, cdq>) {
//         return "cdq";
//     } else if constexpr (std::is_same_v<value_t, Ret>) {
//         return "ret";
//     } else {
//         static_assert(always_false_v<value_t>, "Unhandled instruction");
//     }
// }
// } // namespace detail
//
// inline std::string instruction_to_string(const Instruction &instruction) {
//     return instruction.visit([](const auto &node) {
//         return detail::instruction_to_string_impl(node);
//     });
// }
//
// inline void print_instructions(const std::vector<Instruction> &instructions, std::ostream &os = std::cout) {
//     for (const auto &inst : instructions) {
//         os << instruction_to_string(inst) << '\n';
//     }
// }
// } // namespace compiler::x86::util
