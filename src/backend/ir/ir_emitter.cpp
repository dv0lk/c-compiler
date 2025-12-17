#include "ir_emitter.h"


//TODO maybe the design should something like this:
// we have a function add_instruction. And then this instruction decides if to start/terminate the block based on the current instruction
// ????
namespace compiler::ir {
    Program<instruction> emitter::emit(const std::vector<ast::stmt::stmt_ptr> &ast) {
        for (const auto &stmt: ast) {
            emit_stmt(stmt);
        }

        return program_;
    }

    void emitter::finalize_current_function() {
        if (!current_bb_.empty()) {
            current_function_.add_basic_block(std::move(current_bb_));
            current_bb_ = {};
        }

        if (!current_function_.empty()) {
            program_.add_function(std::move(current_function_));
            current_function_ = {};
        }
    }

    void emitter::start_new_function(const std::string &function_name) {
        finalize_current_function();
        current_function_ = function_t{function_name};
    }

    void emitter::finalize_current_block() {
        if (!current_bb_.empty()) {
            current_function_.add_basic_block(std::move(current_bb_));
        }
    }

    void emitter::start_new_bb(const std::string& label) {
        finalize_current_block();
        current_bb_ = bb_t{};
        current_bb_.add_instruction(ir::label(label));
    }

    [[nodiscard]] std::string emitter::make_tmp_var() {
        return "t" + std::to_string(var_counter_++);
    }

    [[nodiscard]] std::string emitter::make_label(const std::string &label) {
        return label + "_" + std::to_string(label_counter_++);
    }

    [[nodiscard]] std::string emitter::make_variable_name(const std::string &name, size_t scope_id) {
        return name + "_" + std::to_string(scope_id);
    }

    void emitter::emit_stmt(const ast::stmt::stmt_ptr &stmt_var) {
        stmt_var->visit([this](const auto &stmt) {
            emit_stmt(stmt);
        });
    }

    void emitter::emit_stmt(const ast::stmt::return_ &ret) {
        const auto ret_value = emit_expr(ret.value);
        current_bb_.add_instruction(return_{ret_value});
    }

    //TODO uhh is this a bug?
    void emitter::emit_stmt(const ast::stmt::expression &stmt) {
        emit_expr(stmt.expr);
    }

    void emitter::emit_stmt(const ast::stmt::block &block) {
        resolver_.begin_scope();

        if (block.statements.empty()) {
            throw std::runtime_error("Cannot emit empty statement block");
        }

        for (const auto &s: block.statements) {
            emit_stmt(s);
        }

        resolver_.end_scope();
    }

    void emitter::emit_stmt(const ast::stmt::if_ &stmt) {
        const std::string then_label = make_label("if_then");
        const std::string end_label = make_label("if_end");
        const std::string else_label = stmt.else_branch.has_value() ? make_label("if_else") : end_label;

        const VirtualReg cond = emit_expr(stmt.condition);
        current_bb_.add_instruction(jump_if_zero{cond, label{else_label}});

        start_new_bb(then_label);

        emit_stmt(stmt.then_branch);
        if (stmt.else_branch.has_value()) {
            start_new_bb(else_label);
            emit_stmt(stmt.else_branch.value());
            current_bb_.add_instruction(jump{label{end_label}});
        }
        start_new_bb(end_label);
    }

    void emitter::emit_stmt(const ast::stmt::while_ &stmt) {
        const std::string cond_label = make_label("while_cond");
        const std::string body_label = make_label("while_body");
        const std::string end_label = make_label("while_end");

        current_bb_.add_instruction(jump{cond_label});

        start_new_bb(cond_label);
        const VirtualReg condition = emit_expr(stmt.condition);
        current_bb_.add_instruction(jump_if_zero{condition, end_label});
        current_bb_.add_instruction(jump{body_label});

        start_new_bb(body_label);
        emit_stmt(stmt.body);
        current_bb_.add_instruction(jump{cond_label});

        start_new_bb(end_label);
    }

    void emitter::emit_stmt(const ast::stmt::function_param &stmt) {
        throw std::runtime_error("Not implemented\n");
    }

    void emitter::emit_stmt(const ast::stmt::function_decl &func) {
        resolver_.begin_scope();

        //TODO look here
        std::vector<std::string> param_names;
        for (const auto &[name, type]: func.params) {
            resolver_.declare(name);
            param_names.push_back(name);
        }

        start_new_function(func.function_name + "_entry");
        emit_stmt(func.body);
        finalize_current_function();

        resolver_.end_scope();
    }

    void emitter::emit_stmt(const ast::stmt::variable &variable) {
        const auto scope_id = resolver_.declare(variable.name);

        if (variable.initializer.has_value()) {
            const auto rhs = emit_expr(variable.initializer.value());
            const auto lhs = VirtualReg{make_variable_name(variable.name, scope_id.value())};
            current_bb_.add_instruction(copy{lhs, rhs});
            return;
        }

        throw std::runtime_error("Something went wrong");
    }


    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::expr_ptr &expr_var) {
        return expr_var->visit([this](const auto &stmt) {
            return this->emit_expr(stmt);
        });
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::literal &literal) {
        return VirtualReg(literal.value);
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::variable &variable) {
        const auto resolved = resolver_.resolve(variable.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Error resolving variable");
        }

        return VirtualReg(make_variable_name(variable.name, resolved.value()));
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::binary &expr) {
        const VirtualReg left = emit_expr(expr.left);
        const VirtualReg right = emit_expr(expr.right);
        VirtualReg result{make_tmp_var()};

        current_bb_.add_instruction(binary{expr.op, left, right, result});
        return result;
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::unary &expr) {
        const VirtualReg operand = emit_expr(expr.value);
        VirtualReg result{make_tmp_var()};

        current_bb_.add_instruction(unary{expr.op, operand, result});
        return result;
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::grouping &expr) {
        return emit_expr(expr.expr);
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::assignment &expr) {
        const VirtualReg value = emit_expr(expr.value);
        const auto resolved = resolver_.resolve(expr.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Undefined variable assignment");
        }

        VirtualReg destination{make_variable_name(expr.name, resolved.value())};
        current_bb_.add_instruction(copy{destination, value});
        return destination;
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::logical &expr) {
        const std::string short_circuit_label = make_label("short_circuit");
        const std::string end_label = make_label("logical_end");

        VirtualReg left = emit_expr(expr.left);
        VirtualReg result{make_tmp_var()};

        if (expr.op == token_t::LogicalAnd) {
            current_bb_.add_instruction(jump_if_zero{left, short_circuit_label});

            const VirtualReg right = emit_expr(expr.right);
            current_bb_.add_instruction(copy{result, right});
            current_bb_.add_instruction(jump{end_label});

            start_new_bb(short_circuit_label);
            current_bb_.add_instruction(copy{result, VirtualReg(0)});

            start_new_bb(end_label);
        } else if (expr.op == token_t::LogicalOr) {
            current_bb_.add_instruction(jump_if_not_zero{left, short_circuit_label});

            const VirtualReg right = emit_expr(expr.right);
            current_bb_.add_instruction(copy{result, right});
            current_bb_.add_instruction(jump{end_label});

            start_new_bb(short_circuit_label);
            current_bb_.add_instruction(copy{result, VirtualReg(1)});

            start_new_bb(end_label);
        }

        return result;
    }

    [[nodiscard]] VirtualReg emitter::emit_expr(const ast::expr::call &expr) {
        std::vector<VirtualReg> arguments;
        for (const auto &arg: expr.arguments) {
            arguments.push_back(emit_expr(arg));
        }

        VirtualReg result{make_tmp_var()};
        current_bb_.add_instruction(func_call{expr.identifier, arguments, result});

        return result;
    }
}
