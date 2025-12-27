#include <stdexcept>
#include <utility>

#include "codegen.hpp"

compiler::Program<compiler::x86::Instruction> compiler::x86::Emitter::emit(const Program<ir::Instruction>& ir_program) {
    for (const auto& function : ir_program.functions()) {
        assemble_function(function);
    }

    return x86_program_;
}

compiler::x86::Operand compiler::x86::Emitter::convert_virt_reg(const ir::Operand& virtual_reg) {
    if (virtual_reg.is_constant()) {
        return Imm(virtual_reg.get_constant());
    }

    return PseudoRegister(virtual_reg.get_variable());
}

void compiler::x86::Emitter::assemble_function(const Function<ir::Instruction>& function) {
    current_function_ = Function<Instruction>(function.name());

    current_function_.emplace_back(Label(function.name()));
    const auto& params = function.params();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i < ARG_REGISTERS.size()) {
            current_function_.emplace_back(Mov(PseudoRegister(params[i]), Register(ARG_REGISTERS[i])));
        } else {
            int stack_offset = 16 + static_cast<int>((i - ARG_REGISTERS.size()) * 8);
            current_function_.emplace_back(Mov(PseudoRegister(params[i]), Mem(stack_offset)));
        }
    }

    for (const auto& instr : function.instructions()) {
        instr.visit([this](const auto& i) { assemble(i); });
    }

    x86_program_.add_function(std::move(current_function_));
    current_function_ = {};
}

void compiler::x86::Emitter::assemble(const ir::Return& ret) {
    auto ret_value = convert_virt_reg(ret.value);
    current_function_.emplace_back(Mov(Register(RegType::RAX), ret_value));
    current_function_.emplace_back(Ret{});
}

void compiler::x86::Emitter::assemble(const ir::Binary& binary) {
    auto result = convert_virt_reg(binary.result);
    auto left = convert_virt_reg(binary.left);
    auto right = convert_virt_reg(binary.right);

    switch (binary.op) {
        using enum TokenType;
    case Plus:
        current_function_.emplace_back(Mov(result, left));
        current_function_.emplace_back(Add(result, right));
        break;
    case Minus:
        current_function_.emplace_back(Mov(result, left));
        current_function_.emplace_back(Sub(result, right));
        break;
    case Star:
        current_function_.emplace_back(Mov(result, left));
        current_function_.emplace_back(Imul(result, right));
        break;
    case Slash:
        current_function_.emplace_back(Mov(Register(RegType::RAX), left));
        current_function_.emplace_back(Cdq());
        current_function_.emplace_back(Idiv(right));
        current_function_.emplace_back(Mov(result, Register(RegType::RAX)));
        break;
    case Greater:
    case GreaterEqual:
    case Less:
    case LessEqual:
    case EqualEqual:
    case NotEqual: {
        current_function_.emplace_back(Cmp(left, right));
        current_function_.emplace_back(Mov(result, Imm{0}));

        CC cc = CC::Empty;
        switch (binary.op) {
        case Greater:
            cc = CC::Greater;
            break;
        case GreaterEqual:
            cc = CC::GreaterOrEqual;
            break;
        case Less:
            cc = CC::Less;
            break;
        case LessEqual:
            cc = CC::LessOrEqual;
            break;
        case EqualEqual:
            cc = CC::Equal;
            break;
        case NotEqual:
            cc = CC::NotEqual;
            break;
        default:
            std::unreachable();
        }
        current_function_.emplace_back(SetCC(cc, result));
        break;
    }
    default:
        throw std::runtime_error("Unknown binary operation");
    }
}

void compiler::x86::Emitter::assemble(const ir::Unary& unary) {
    auto result = convert_virt_reg(unary.result);
    auto value = convert_virt_reg(unary.value);

    switch (unary.op) {
        using enum TokenType;
    case Minus:
        current_function_.emplace_back(Neg(result));
        break;
    case Tilde:
        current_function_.emplace_back(x86::Not(result));
        break;
    case Not:
        current_function_.emplace_back(Cmp(value, Imm{0}));
        current_function_.emplace_back(Mov(result, Imm{0}));
        current_function_.emplace_back(SetCC(CC::Equal, result));
        break;
    default:
        throw std::runtime_error("Unknown unary operation");
    }
}

void compiler::x86::Emitter::assemble(const ir::Copy& copy) {
    auto dest = convert_virt_reg(copy.destination);
    auto source = convert_virt_reg(copy.source);

    if (dest == source) {
        return;
    }

    current_function_.emplace_back(Mov(dest, source));
}

void compiler::x86::Emitter::assemble(const ir::Label& label) {
    current_function_.emplace_back(Label{label.name});
}

void compiler::x86::Emitter::assemble(const ir::Jump& jump) {
    current_function_.emplace_back(Jmp{jump.target.name});
}

void compiler::x86::Emitter::assemble(const ir::JumpIfZero& jump_if_zero) {
    auto condition = convert_virt_reg(jump_if_zero.condition);
    current_function_.emplace_back(Cmp{condition, Imm{0}});
    current_function_.emplace_back(JmpCC{CC::Equal, jump_if_zero.target_label.name});
}

void compiler::x86::Emitter::assemble(const ir::JumpIfNotZero& jump_if_not_zero) {
    auto condition = convert_virt_reg(jump_if_not_zero.condition);
    current_function_.emplace_back(Cmp{condition, Imm{0}});
    current_function_.emplace_back(JmpCC{CC::NotEqual, jump_if_not_zero.target_label.name});
}

void compiler::x86::Emitter::assemble(const ir::FunctionCall& func_call) {
    const auto& args = func_call.arguments;

    // TODO currently we only support max 6 arguments
    for (int i = 0; i < args.size(); i++) {
        current_function_.emplace_back(Mov(Register(ARG_REGISTERS[i]), convert_virt_reg(args[i])));
    }

    current_function_.emplace_back(Call{LabelOp{func_call.function_name + "_entry"}});

    auto dest = convert_virt_reg(func_call.destination);
    current_function_.emplace_back(Mov(dest, Register(RegType::RAX)));
}
