#include <stdexcept>
#include "codegen.hpp"

compiler::Program<compiler::x86::Instruction> compiler::x86::emitter::emit(const Program<ir::Instruction> &ir_program) {
    for (const auto &function: ir_program.functions()) {
        assemble_function(function);
    }

    return x86_program_;
}


compiler::x86::Operand compiler::x86::emitter::convert_virt_reg(const ir::Operand &virtual_reg) {
    if (virtual_reg.is_constant()) {
        return Imm(virtual_reg.get_constant());
    }

    return PseudoRegister(virtual_reg.get_variable());
}

void compiler::x86::emitter::assemble_function(const Function<ir::Instruction> &function) {
    current_function_ = Function<Instruction>(function.name());

    current_bb_.emplace_back(Label(function.name()));

    for (const auto &block: function.basic_blocks()) {
        assemble_bb(block);
    }

    x86_program_.add_function(std::move(current_function_));
    current_function_ = {};
}

void compiler::x86::emitter::assemble_bb(const BasicBlock<ir::Instruction> &block) {
    current_bb_ = BasicBlock<Instruction>();

    for (const auto &instr: block.instructions()) {
        instr.visit([this](const auto &i) { assemble(i); });
    }

    current_function_.add_bb(std::move(current_bb_));
    current_bb_ = {};
}

void compiler::x86::emitter::assemble(const ir::Return &ret) {
    auto ret_value = convert_virt_reg(ret.value);
    current_bb_.emplace_back(Mov(Register(RegType::RAX), ret_value));
    current_bb_.emplace_back(Ret{});
}

void compiler::x86::emitter::assemble(const ir::Binary &binary) {
    auto result = convert_virt_reg(binary.result);
    auto left = convert_virt_reg(binary.left);
    auto right = convert_virt_reg(binary.right);

    switch (binary.op) {
            using enum TokenType;
        case Plus:
            current_bb_.emplace_back(Mov(result, left));
            current_bb_.emplace_back(Add(result, right));
            break;
        case Minus:
            current_bb_.emplace_back(Mov(result, left));
            current_bb_.emplace_back(Sub(result, right));
            break;
        case Star:
            current_bb_.emplace_back(Mov(result, left));
            current_bb_.emplace_back(Imul(result, right));
            break;
        case Slash:
            current_bb_.emplace_back(Mov(Register(RegType::RAX), left));
            current_bb_.emplace_back(cdq());
            current_bb_.emplace_back(Idiv(right));
            current_bb_.emplace_back(Mov(result, Register(RegType::RAX)));
            break;
        case Greater:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::Greater, result));
            break;
        case GreaterEqual:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::GreaterOrEqual, result));
            break;
        case Less:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::Less, result));
            break;
        case LessEqual:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::LessOrEqual, result));
            break;
        case EqualEqual:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::Equal, result));
            break;
        case NotEqual:
            current_bb_.emplace_back(Cmp(left, right));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::NotEqual, result));
            break;
        default: throw std::runtime_error("Unknown binary operation");
    }
}

void compiler::x86::emitter::assemble(const ir::Unary &unary) {
    auto result = convert_virt_reg(unary.result);
    auto value = convert_virt_reg(unary.value);

    switch (unary.op) {
            using enum TokenType;
        case Minus:
            current_bb_.emplace_back(Neg(result));
            break;
        case Tilde:
            current_bb_.emplace_back(x86::Not(result));
            break;
        case Not:
            current_bb_.emplace_back(Cmp(value, Imm{0}));
            current_bb_.emplace_back(Mov(result, Imm{0}));
            current_bb_.emplace_back(SetCC(CC::Equal, result));
            break;
        default:
            throw std::runtime_error("Unknown unary operation");
    }
}

void compiler::x86::emitter::assemble(const ir::Copy &copy) {
    auto dest = convert_virt_reg(copy.destination);
    auto source = convert_virt_reg(copy.source);

    if (dest == source) {
        return;
    }

    current_bb_.emplace_back(Mov(dest, source));
}

void compiler::x86::emitter::assemble(const ir::Label &label) {
    current_bb_.emplace_back(Label{label.name});
}

void compiler::x86::emitter::assemble(const ir::Jump &jump) {
    current_bb_.emplace_back(Jmp{jump.target.name});
}

void compiler::x86::emitter::assemble(const ir::JumpIfZero &jump_if_zero) {
    auto condition = convert_virt_reg(jump_if_zero.condition);
    current_bb_.emplace_back(Cmp{condition, Imm{0}});
    current_bb_.emplace_back(JmpCC{CC::Equal, jump_if_zero.target_label.name});
}

void compiler::x86::emitter::assemble(const ir::JumpIfNotZero &jump_if_not_zero) {
    auto condition = convert_virt_reg(jump_if_not_zero.condition);
    current_bb_.emplace_back(Cmp{condition, Imm{0}});
    current_bb_.emplace_back(JmpCC{CC::NotEqual, jump_if_not_zero.target_label.name});
}

void compiler::x86::emitter::assemble(const ir::FunctionCall &func_call) {
    for (auto it = func_call.arguments.rbegin(); it != func_call.arguments.rend(); ++it) {
        current_bb_.emplace_back(Push{convert_virt_reg(*it)});
    }

    current_bb_.emplace_back(Call{func_call.function_name});

    if (!func_call.arguments.empty()) {
        current_bb_.emplace_back(
            Add{Register(RegType::RSP), Imm{static_cast<int>(func_call.arguments.size() * 8)}});
    }

    auto dest = convert_virt_reg(func_call.destination);
    current_bb_.emplace_back(Mov(dest, Register(RegType::RAX)));
}
