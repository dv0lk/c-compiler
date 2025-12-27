#include "emitter.hpp"

namespace compiler::ir {
    Program<Instruction> Emitter::emit(const std::vector<ast::stmt::stmt_ptr> &ast) {
        for (const auto &stmt: ast) {
            emit_stmt(stmt);
        }

        return program_;
    }

    void Emitter::finalize_current_function() {
        if (!current_function_.empty()) {
            program_.add_function(std::move(current_function_));
            current_function_ = {};
        }
    }

    void Emitter::start_new_function(const std::string &function_name) {
        finalize_current_function();
        current_function_ = function_t{function_name};
    }

    [[nodiscard]] std::string Emitter::make_tmp_var() {
        return "t" + std::to_string(var_counter_++);
    }

    [[nodiscard]] std::string Emitter::make_label(const std::string &label) {
        return label + "_" + std::to_string(label_counter_++);
    }

    [[nodiscard]] std::string Emitter::make_variable_name(const std::string &name, size_t scope_id) {
        return name + "_" + std::to_string(scope_id);
    }

    void Emitter::emit_stmt(const ast::stmt::stmt_ptr &stmt_var) {
        stmt_var->visit([this](const auto &stmt) {
            emit_stmt(stmt);
        });
    }

    void Emitter::emit_stmt(const ast::stmt::return_ &ret) {
        const auto ret_value = emit_expr(ret.value);
        current_function_.emplace_back(Return{ret_value});
    }

    //TODO uhh is this a bug?
    void Emitter::emit_stmt(const ast::stmt::expression &stmt) {
        emit_expr(stmt.expr);
    }

    void Emitter::emit_stmt(const ast::stmt::block &block) {
        resolver_.begin_scope();

        if (block.statements.empty()) {
            throw std::runtime_error("Empty statement block");
        }

        for (const auto &s: block.statements) {
            emit_stmt(s);
        }

        resolver_.end_scope();
    }

    void Emitter::emit_stmt(const ast::stmt::if_ &stmt) {
        const std::string then_label = make_label("if_then");
        const std::string end_label = make_label("if_end");
        const std::string else_label = stmt.else_branch.has_value() ? make_label("if_else") : end_label;

        auto cond = emit_expr(stmt.condition);
        current_function_.emplace_back(JumpIfZero{cond, Label{else_label}});

        current_function_.emplace_back(Label{then_label});

        emit_stmt(stmt.then_branch);
        if (stmt.else_branch.has_value()) {
            current_function_.emplace_back(Label{else_label});
            emit_stmt(stmt.else_branch.value());
            current_function_.emplace_back(Jump{Label{end_label}});
        }
        current_function_.emplace_back(Label{end_label});
    }

    void Emitter::emit_stmt(const ast::stmt::while_ &stmt) {
        const std::string cond_label = make_label("while_cond");
        const std::string body_label = make_label("while_body");
        const std::string end_label = make_label("while_end");

        current_function_.emplace_back(Jump{cond_label});

        current_function_.emplace_back(Label{cond_label});
        auto condition = emit_expr(stmt.condition);
        current_function_.emplace_back(JumpIfNotZero{condition, end_label});
        current_function_.emplace_back(Jump{body_label});

        current_function_.emplace_back(Label{body_label});
        emit_stmt(stmt.body);
        current_function_.emplace_back(Jump{cond_label});

        current_function_.emplace_back(Label{end_label});
    }

    void Emitter::emit_stmt(const ast::stmt::function_param &param) {
        const auto scope_id = resolver_.declare(param.name);
        current_function_.add_param(make_variable_name(param.name, scope_id.value()));
    }

    void Emitter::emit_stmt(const ast::stmt::function_decl &func) {
        start_new_function(func.function_name + "_entry");
        resolver_.begin_scope();

        for (const auto &param : func.params) {
            emit_stmt(param);
        }

        emit_stmt(func.body);
        finalize_current_function();

        resolver_.end_scope();
    }

    void Emitter::emit_stmt(const ast::stmt::variable &variable) {
        const auto scope_id = resolver_.declare(variable.name);

        if (variable.initializer.has_value()) {
            const auto rhs = emit_expr(variable.initializer.value());
            const auto lhs = Operand{make_variable_name(variable.name, scope_id.value())};
            current_function_.emplace_back(Copy{lhs, rhs});
            return;
        }

        throw std::runtime_error("Something went wrong");
    }


    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::expr_ptr &expr_var) {
        return expr_var->visit([this](const auto &stmt) {
            return this->emit_expr(stmt);
        });
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::literal &literal) {
        return literal.value;
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::variable &variable) {
        const auto resolved = resolver_.resolve(variable.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Error resolving variable");
        }

        return {make_variable_name(variable.name, resolved.value())};
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::binary &expr) {
        auto left = emit_expr(expr.left);
        auto right = emit_expr(expr.right);
        auto result = Operand{make_tmp_var()};

        current_function_.emplace_back(Binary{expr.op, left, right, result});
        return result;
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::unary &expr) {
        auto operand = emit_expr(expr.value);
        auto result = Operand(make_tmp_var());

        current_function_.emplace_back(Unary{expr.op, operand, result});
        return result;
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::grouping &expr) {
        return emit_expr(expr.expr);
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::assignment &expr) {
        auto value = emit_expr(expr.value);
        const auto resolved = resolver_.resolve(expr.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Undefined variable assignment");
        }

        auto destination= Operand(make_variable_name(expr.name, resolved.value()));
        current_function_.emplace_back(Copy{destination, value});
        return destination;
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::logical &expr) {
        const std::string short_circuit_label = make_label("short_circuit");
        const std::string end_label = make_label("logical_end");

        auto left = emit_expr(expr.left);
        auto result= Operand(make_tmp_var());

        if (expr.op == TokenType::LogicalAnd) {
            current_function_.emplace_back(JumpIfZero{left, short_circuit_label});

            auto right = emit_expr(expr.right);
            current_function_.emplace_back(Copy{result, right});
            current_function_.emplace_back(Jump{end_label});

            current_function_.emplace_back(Label{short_circuit_label});
            current_function_.emplace_back(Copy{result, 0});

            current_function_.emplace_back(Label{end_label});
        } else if (expr.op == TokenType::LogicalOr) {
            current_function_.emplace_back(JumpIfNotZero{left, short_circuit_label});

            auto right = emit_expr(expr.right);
            current_function_.emplace_back(Copy{result, right});
            current_function_.emplace_back(Jump{end_label});

            current_function_.emplace_back(Label{short_circuit_label});
            current_function_.emplace_back(Copy{result, 1});

            current_function_.emplace_back(Label{end_label});
        }

        return result;
    }

    [[nodiscard]] Operand Emitter::emit_expr(const ast::expr::call &expr) {
        std::vector<Operand> arguments;
        for (const auto &arg: expr.arguments) {
            arguments.push_back(emit_expr(arg));
        }

        auto result = Operand(make_tmp_var());
        current_function_.emplace_back(FunctionCall{expr.identifier, arguments, result});

        return result;
    }
}
