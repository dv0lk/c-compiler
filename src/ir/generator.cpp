#include "generator.h"


//TODO maybe the design should something like this:
// we have a function add_instruction. And then this instruction decides if to start/terminate the block based on the current instruction
// ????
namespace compiler::ir {
    std::vector<basic_block_t> generator::generate(const std::vector<ast::stmt::stmt_ptr> &ast) {
        current_block_ = basic_block_t("entry");

        for (const auto &stmt: ast) {
            emit_stmt(stmt);
        }

        finalize_current_block();

        return blocks_;
    }

    void generator::finalize_current_block() {
        if (!current_block_.empty()) {
            blocks_.push_back(std::move(current_block_));
        }
    }

    void generator::start_new_bb(std::string label) {
        finalize_current_block();
        current_block_ = basic_block_t{std::move(label)};
    }

    [[nodiscard]] std::string generator::make_tmp_var() {
        return "t" + std::to_string(var_counter_++);
    }

    [[nodiscard]] std::string generator::make_label(const std::string &label) {
        return label + "_" + std::to_string(label_counter_++);
    }

    [[nodiscard]] std::string generator::make_variable_name(const std::string &name, size_t scope_id) {
        return name + "_" + std::to_string(scope_id);
    }

    void generator::emit_stmt(const ast::stmt::stmt_ptr &stmt_var) {
        stmt_var->visit([this](const auto &stmt) {
            emit_stmt(stmt);
        });
    }

    void generator::emit_stmt(const ast::stmt::return_ &ret) {
        const auto ret_value = emit_expr(ret.value);
        current_block_.append(return_{ret_value});
    }

    void generator::emit_stmt(const ast::stmt::expression &stmt) {
        emit_expr(stmt.expr);
    }

    void generator::emit_stmt(const ast::stmt::block &block) {
        resolver_.begin_scope();

        if (block.statements.empty()) {
                        throw std::runtime_error("Cannot emit empty statement block");
        }

        for (const auto &s: block.statements) {
            emit_stmt(s);
        }

        resolver_.end_scope();
    }

    void generator::emit_stmt(const ast::stmt::if_ &stmt) {
        const std::string then_label = make_label("if_then");
        const std::string end_label = make_label("if_end");
        const std::string else_label = stmt.else_branch.has_value() ? make_label("if_else") : end_label;

        const value_t cond = emit_expr(stmt.condition);
        current_block_.append(jump_if_zero{cond, label{else_label}});

        start_new_bb(then_label);

        emit_stmt(stmt.then_branch);

        if (stmt.else_branch.has_value()) {
            start_new_bb(else_label);
            emit_stmt(stmt.else_branch.value());
            current_block_.append(jump{label{end_label}});
        }
        start_new_bb(end_label);
    }

    void generator::emit_stmt(const ast::stmt::while_ &stmt) {
        const std::string cond_label = make_label("while_cond");
        const std::string body_label = make_label("while_body");
        const std::string end_label = make_label("while_end");

        current_block_.append(jump{cond_label});

        start_new_bb(cond_label);
        const value_t condition = emit_expr(stmt.condition);
        current_block_.append(jump_if_zero{condition, end_label});
        current_block_.append(jump{body_label});

        start_new_bb(body_label);
        emit_stmt(stmt.body);
        current_block_.append(jump{cond_label});

        start_new_bb(end_label);
    }

    void generator::emit_stmt(const ast::stmt::function_param &stmt) {
        throw std::runtime_error("Not implemented\n");
    }

    void generator::emit_stmt(const ast::stmt::function_decl &func) {
        resolver_.begin_scope();

        for (const auto &[name, type]: func.params) {
            resolver_.declare(name);
        }

        start_new_bb(func.function_name + "_entry");
        emit_stmt(func.body);

        start_new_bb("entry");
        resolver_.end_scope();
    }

    void generator::emit_stmt(const ast::stmt::variable &variable) {
        const auto scope_id = resolver_.declare(variable.name);

        if (variable.initializer.has_value()) {
            const auto rhs = emit_expr(variable.initializer.value());
            const auto lhs = value_t{make_variable_name(variable.name, scope_id.value())};
            current_block_.append(copy{lhs, rhs});
            return;
        }

        throw std::runtime_error("Something went wrong");
    }


    [[nodiscard]] value_t generator::emit_expr(const ast::expr::expr_ptr &expr_var) {
        return expr_var->visit([this](const auto &stmt) {
            return this->emit_expr(stmt);
        });
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::literal &literal) {
        return value_t(literal.value);
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::variable &variable) {
        const auto resolved = resolver_.resolve(variable.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Error resolving variable");
        }

        return value_t(make_variable_name(variable.name, resolved.value()));
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::binary &expr) {
        const value_t left = emit_expr(expr.left);
        const value_t right = emit_expr(expr.right);
        value_t result{make_tmp_var()};

        current_block_.append(binary{expr.op, left, right, result});
        return result;
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::unary &expr) {
        const value_t operand = emit_expr(expr.value);
        value_t result{make_tmp_var()};

        current_block_.append(unary{expr.op, operand, result});
        return result;
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::grouping &expr) {
        return emit_expr(expr.expr);
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::assignment &expr) {
        const value_t value = emit_expr(expr.value);
        const auto resolved = resolver_.resolve(expr.name);

        if (!resolved.has_value()) {
            throw std::runtime_error("Undefined variable assignment");
        }

        value_t destination{make_variable_name(expr.name, resolved.value())};
        current_block_.append(copy{destination, value});
        return destination;
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::logical &expr) {
        const std::string short_circuit_label = make_label("short_circuit");
        const std::string end_label = make_label("logical_end");

        value_t left = emit_expr(expr.left);
        value_t result{make_tmp_var()};

        if (expr.op == token_t::LogicalAnd) {
            current_block_.append(jump_if_zero{left, short_circuit_label});

            const value_t right = emit_expr(expr.right);
            current_block_.append(copy{result, right});
            current_block_.append(jump{end_label});

            start_new_bb(short_circuit_label);
            current_block_.append(copy{result, value_t(0)});

            start_new_bb(end_label);
        } else if (expr.op == token_t::LogicalOr) {
            current_block_.append(jump_if_not_zero{left, short_circuit_label});

            const value_t right = emit_expr(expr.right);
            current_block_.append(copy{result, right});
            current_block_.append(jump{end_label});

            start_new_bb(short_circuit_label);
            current_block_.append(copy{result, value_t(1)});

            start_new_bb(end_label);
        }

        return result;
    }

    [[nodiscard]] value_t generator::emit_expr(const ast::expr::call &expr) {
        std::vector<value_t> arguments;
        for (const auto &arg: expr.arguments) {
            arguments.push_back(emit_expr(arg));
        }

        value_t result{make_tmp_var()};
        current_block_.append(func_call{expr.identifier, arguments, result});

        return result;
    }
}
