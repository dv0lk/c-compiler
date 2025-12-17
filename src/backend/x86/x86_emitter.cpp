#include "x86_emitter.hpp"

#include <stdexcept>

compiler::Program<compiler::x86::Instruction> compiler::x86::emitter::emit(const Program<ir::instruction> &ir_program) {
    for (const auto &function: ir_program.functions()) {
        assemble_function(function);
    }

    return x86_program_;
}


compiler::x86::Operand compiler::x86::emitter::convert_virt_reg(const ir::VirtualReg &virtual_reg) {
    if (virtual_reg.is_constant()) {
        return Imm(virtual_reg.get_constant());
    }

    return PseudoRegister(virtual_reg.get_variable());
}

void compiler::x86::emitter::assemble_function(const Function<ir::instruction> &function) {
    current_function_ = Function<Instruction>(function.name());

    current_bb_.add_instruction(Label(LabelOperand(function.name())));

    for (const auto &block: function.basic_blocks()) {
        assemble_bb(block);
    }

    x86_program_.add_function(std::move(current_function_));
    current_function_ = {};
}

void compiler::x86::emitter::assemble_bb(const BasicBlock<ir::instruction> &block) {
    current_bb_ = BasicBlock<Instruction>();

    for (const auto &instr: block.instructions()) {
        instr.visit([this](const auto &i) { assemble(i); });
    }

    current_function_.add_basic_block(std::move(current_bb_));
    current_bb_ = {};
}

void compiler::x86::emitter::assemble(const ir::return_ &ret) {
    auto ret_value = convert_virt_reg(ret.value);
    current_bb_.add_instruction(Mov(Register(register_t::RAX), ret_value));
    current_bb_.add_instruction(Ret{});
}

void compiler::x86::emitter::assemble(const ir::binary &binary) {
    auto result = convert_virt_reg(binary.result);
    auto left = convert_virt_reg(binary.left);
    auto right = convert_virt_reg(binary.right);

    switch (binary.op) {
            using enum token_t;
        case Plus:
            current_bb_.add_instruction(Mov(result, left));
            current_bb_.add_instruction(Add(result, right));
            break;
        case Minus:
            current_bb_.add_instruction(Mov(result, left));
            current_bb_.add_instruction(Sub(result, right));
            break;
        case Star:
            current_bb_.add_instruction(Mov(result, left));
            current_bb_.add_instruction(Imul(result, right));
            break;
        case Slash:
            current_bb_.add_instruction(Mov(Register(register_t::RAX), left));
            current_bb_.add_instruction(cdq());
            current_bb_.add_instruction(Idiv(right));
            current_bb_.add_instruction(Mov(result, Register(register_t::RAX)));
            break;
        case Greater:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::Greater, result));
            break;
        case GreaterEqual:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::GreaterOrEqual, result));
            break;
        case Less:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::Less, result));
            break;
        case LessEqual:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::LessOrEqual, result));
            break;
        case EqualEqual:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::Equal, result));
            break;
        case NotEqual:
            current_bb_.add_instruction(Cmp(left, right));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::NotEqual, result));
            break;
        default: throw std::runtime_error("Unknown binary operation");
    }
}

void compiler::x86::emitter::assemble(const ir::unary &unary) {
    auto result = convert_virt_reg(unary.result);
    auto value = convert_virt_reg(unary.value);

    switch (unary.op) {
            using enum token_t;
        case Minus:
            current_bb_.add_instruction(Neg(result));
            break;
        case Tilde:
            current_bb_.add_instruction(x86::Not(result));
            break;
        case Not:
            current_bb_.add_instruction(Cmp(value, Imm{0}));
            current_bb_.add_instruction(Mov(result, Imm{0}));
            current_bb_.add_instruction(SetCC(CC::Equal, result));
            break;
        default:
            throw std::runtime_error("Unknown unary operation");
    }
}

void compiler::x86::emitter::assemble(const ir::copy &copy) {
    auto dest = convert_virt_reg(copy.destination);
    auto source = convert_virt_reg(copy.source);

    current_bb_.add_instruction(Mov(dest, source));
}

void compiler::x86::emitter::assemble(const ir::label &label) {
    current_bb_.add_instruction(Label{LabelOperand{label.name}});
}

void compiler::x86::emitter::assemble(const ir::jump &jump) {
    current_bb_.add_instruction(Jmp{LabelOperand{jump.target.name}});
}

void compiler::x86::emitter::assemble(const ir::jump_if_zero &jump_if_zero) {
    auto condition = convert_virt_reg(jump_if_zero.condition);
    current_bb_.add_instruction(Cmp{condition, Imm{0}});
    current_bb_.add_instruction(JmpCC{CC::Equal, LabelOperand{jump_if_zero.target_label.name}});
}

void compiler::x86::emitter::assemble(const ir::jump_if_not_zero &jump_if_not_zero) {
    auto condition = convert_virt_reg(jump_if_not_zero.condition);
    current_bb_.add_instruction(Cmp{condition, Imm{0}});
    current_bb_.add_instruction(JmpCC{CC::NotEqual, LabelOperand{jump_if_not_zero.target_label.name}});
}

void compiler::x86::emitter::assemble(const ir::func_call &func_call) {
    for (auto it = func_call.arguments.rbegin(); it != func_call.arguments.rend(); ++it) {
        current_bb_.add_instruction(Push{convert_virt_reg(*it)});
    }

    current_bb_.add_instruction(Call{LabelOperand{func_call.function_name}});

    if (!func_call.arguments.empty()) {
        current_bb_.add_instruction(
            Add{Register(register_t::RSP), Imm{static_cast<int>(func_call.arguments.size() * 8)}});
    }

    auto dest = convert_virt_reg(func_call.destination);
    current_bb_.add_instruction(Mov(dest, Register(register_t::RAX)));
}
