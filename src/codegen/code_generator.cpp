#include "code_generator.hpp"

#include <iostream>

#include <print>

namespace compiler {

    void CodeGenerator::generate(const std::vector<ir::ir_instruction>& ir_instructions) {
        for (auto& instruction : ir_instructions) {
            std::visit([this](auto& inst) {
                this->assemble(inst);
            }, instruction);
        }

        std::println();
        std::println("Compiled: ");
        for (auto& instr : instructions) {
            std::visit([](auto& value) {
                std::println("{}", value.emit());
            }, instr);
        }

    }

    x86::Operand CodeGenerator::convert_value(const ir::ir_value& value) {
        if (value.is_constant())
            return x86::Imm{value.get<int>()};

        const auto& var_name = value.get<std::string>();
        //TODO temp fix
        if (var_name.starts_with("t")) {
            return x86::PseudoRegister(var_name);
        } else {
            return x86::Mem(get_temp_location(var_name));
        }

    }

    int CodeGenerator::get_temp_location(const std::string& name) {
        const auto it = temp_var_locations.find(name);

        if (it != temp_var_locations.end()) {
            return it->second;
        }
        const int offset = current_stack_offset;
        temp_var_locations[name] = offset;
        current_stack_offset -= 4;
        return offset;
    }

    void CodeGenerator::assemble(const ir::ir_binary& binary) {
        auto result = convert_value(binary.result);
        auto left = convert_value(binary.left);
        auto right = convert_value(binary.right);

        switch (binary.op) {
            using enum token_type;
        case Plus:
            add_instruction(x86::mov{left, result});
            add_instruction(x86::add{right, result});
            break;
        case Minus:
            add_instruction(x86::mov{left, result});
            add_instruction(x86::sub{right, result});
            break;
        case Star:
            add_instruction(x86::mov{left, result});
            add_instruction(x86::imul{right, result});
            break;
        case Slash:
            add_instruction(x86::mov{left, x86::registers::RAX});
            add_instruction(x86::cdq{});
            add_instruction(x86::idiv{right});
            add_instruction(x86::mov{x86::registers::RAX, result});
        case Greater:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::Greater, result});
            break;
        case GreaterEqual:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::GreaterOrEqual, result});
            break;
        case Less:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::Less, result});
            break;
        case LessEqual:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::LessOrEqual, result});
            break;
        case EqualEqual:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::Equal, result});
            break;
        case NotEqual:
            add_instruction(x86::cmp{right, left});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::NotEqual, result});
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
        }
    }

    void CodeGenerator::assemble(const ir::ir_return& ret) {
        const auto source = convert_value(ret.value);
        add_instruction(x86::mov{source, x86::registers::RAX});
        add_instruction(x86::ret{});
    }

    void CodeGenerator::assemble(const ir::ir_unary& unary) {
        const auto value = convert_value(unary.value);
        const auto result = convert_value(unary.result);

        if (unary.value != unary.result) {
            add_instruction(x86::mov{value, result});
        }

        switch (unary.op) {
            using enum token_type;
        case Tilde:
            add_instruction(x86::not_{result});
            break;
        case Minus:
            add_instruction(x86::neg{result});
            break;
        case Not:
            add_instruction(x86::cmp{x86::Imm{0}, value});
            add_instruction(x86::mov{x86::Imm{0}, result});
            add_instruction(x86::set_cc{x86::CC::Equal, result});
            break;
        default:
            throw std::runtime_error("Unsupported unary operator");
        }
    }

    void CodeGenerator::assemble(const ir::ir_copy& copy) {
        const auto destination = convert_value(copy.destination);
        const auto source = convert_value(copy.source);
        add_instruction(x86::mov{source, destination});
    }

    void CodeGenerator::assemble(const ir::ir_label& label) {
        //TODO scuffed?
        add_instruction(x86::label{x86::Label{label.name}});
    }

    void CodeGenerator::assemble(const ir::ir_jump& jump) {
        add_instruction(x86::jmp{x86::Label{jump.label.name}});
    }

    void CodeGenerator::assemble(const ir::ir_jump_if_zero& jump) {
        const auto condition = convert_value(jump.condition);

        add_instruction(x86::mov{condition, x86::registers::RAX});
        add_instruction(x86::cmp{x86::Imm{0}, x86::registers::RAX});
        add_instruction(x86::jmp_cc{x86::CC::Equal, x86::Label{jump.label.name}});
    }

    void CodeGenerator::assemble(const ir::ir_jump_if_not_zero& jump) {
        const auto condition = convert_value(jump.condition);

        add_instruction(x86::mov{condition, x86::registers::RAX});
        add_instruction(x86::cmp{x86::Imm{0}, x86::registers::RAX});
        add_instruction(x86::jmp_cc{x86::CC::NotEqual, x86::Label{jump.label.name}});
    }

    void CodeGenerator::assemble(const ir::ir_call& call) {
        throw std::runtime_error("eror");
    }

}
