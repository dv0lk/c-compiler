#include "generator.h"

//TODO start_new_block
namespace compiler::ir {
    std::vector<basic_block_t> generator::generate(const std::vector<ast::stmt::stmt_ptr> &ast) {
        curr_bb = basic_block_t("entry");

        for (const auto &stmt: ast) {
            process_stmt(stmt);
        }

        if (!curr_bb.empty())
            bbs.push_back(curr_bb);

        return bbs;
    }

    void generator::start_new_bb(std::string label) {
        if (!curr_bb.empty()) {
            bbs.emplace_back(curr_bb);
        }
        curr_bb = basic_block_t{std::move(label)};
    }

    std::string generator::generate_tmp() {
        return "t" + std::to_string(temp_var_counter++);
    }

    std::string generator::get_label(const std::string &label) {
        static int count = 0;
        return label + "_" + std::to_string(count++);
    }

    void generator::process_stmt(const ast::stmt::stmt_ptr &stmt_var) {
        return stmt_var->visit([this](const auto &stmt) {
            return this->process_stmt(stmt);
        });
    }

    value_t generator::process_expr(const ast::expr_ptr &expr_var) {
        return expr_var->visit([this](const auto &expr) {
            return this->process_expr(expr);
        });
    }

    void generator::process_stmt(const ast::stmt::return_ &ret) {
        const value_t return_value = process_expr(ret.value);
        curr_bb.push_instr(return_{return_value});
    }

    void generator::process_stmt(const ast::stmt::expression &stmt) {
        process_expr(stmt.expr);
    }

    void generator::process_stmt(const ast::stmt::block &block) {
        resolver.begin_scope();

        if (block.statements.empty()) {
            throw std::runtime_error("Empty block?");
        }

        for (const auto &s: block.statements) {
            process_stmt(s);
        }

        resolver.end_scope();
    }

    void generator::process_stmt(const ast::stmt::if_ &stmt) {
        const std::string then_label = get_label("if_then");
        const std::string end_label = get_label("if_end");
        const std::string else_label = stmt.else_branch.has_value() ? get_label("if_else") : end_label;

        const value_t cond = process_expr(stmt.condition);

        curr_bb.push_instr(jump_if_zero{cond, label{else_label}});
        bbs.push_back(std::move(curr_bb));

        curr_bb = basic_block_t{then_label};
        process_stmt(stmt.then_branch);

        // current_block.push_instr(jump{label{end_label}}); //???
        bbs.push_back(std::move(curr_bb));

        if (stmt.else_branch.has_value()) {
            curr_bb = basic_block_t{else_label};
            process_stmt(stmt.else_branch.value());
            curr_bb.push_instr(jump{label{end_label}});
            bbs.push_back(std::move(curr_bb));
        }
        curr_bb = basic_block_t{end_label};


        // const std::string else_label = get_label("else");
        // const std::string end_label = get_label("end");
        // const value_t condition = process_expr(stmt.condition);
        // current_block.push_instr(jump_if_zero{condition, else_label});
        //
        // process_stmt(stmt.then_branch);
        //
        // if (stmt.else_branch.has_value()) {
        //     current_block.push_instr(jump{end_label});
        //     current_block.push_instr(label{else_label});
        //     process_stmt(stmt.else_branch.value());
        //     current_block.push_instr(label{end_label});
        // } else {
        //     current_block.push_instr(label{else_label});
        // }
    }

    void generator::process_stmt(const ast::stmt::while_ &stmt) {
        const std::string cond_label = get_label("while_cond");
        const std::string body_label = get_label("while_body");
        const std::string end_label = get_label("while_end");

        curr_bb.push_instr(jump{cond_label});
        bbs.push_back(curr_bb);

        curr_bb = basic_block_t{cond_label};

        const value_t condition = process_expr(stmt.condition);
        curr_bb.push_instr(jump_if_zero{condition, end_label});
        curr_bb.push_instr(jump{body_label});
        bbs.push_back(curr_bb);

        curr_bb = basic_block_t{body_label};
        process_stmt(stmt.body);
        curr_bb.push_instr(jump{cond_label});
        bbs.push_back(curr_bb);

        curr_bb = basic_block_t{end_label};
    }

    value_t generator::process_expr(const ast::literal_expr &literal) {
        return value_t(literal.value);
    }

    value_t generator::process_expr(const ast::variable_expr &variable) {
        const auto resolved = resolver.resolve(variable.name);

        if (!resolved.has_value())
            throw std::runtime_error("Error resolving variable\n");

        const std::string name = variable.name + "_" + std::to_string(resolved.value());
        return value_t(name);
    }

    value_t generator::process_expr(const ast::binary_expr &expr) {
        const value_t left = process_expr(expr.left);
        const value_t right = process_expr(expr.right);
        value_t result{generate_tmp()};

        curr_bb.push_instr(binary{expr.op, left, right, result});
        return result;
    }

    value_t generator::process_expr(const ast::unary_expr &expr) {
        const value_t operand = process_expr(expr.value);
        value_t result{generate_tmp()};

        curr_bb.push_instr(unary{expr.op, operand, result});
        return result;
    }

    value_t generator::process_expr(const ast::grouping_expr &expr) {
        return process_expr(expr.expr);
    }

    value_t generator::process_expr(const ast::assignment_expr &expr) {
        const value_t value = process_expr(expr.value);
        const auto resolved = resolver.resolve(expr.name);
        if (!resolved.has_value())
            throw std::runtime_error("Undefined variable assignment");

        value_t destination{expr.name + "_" + std::to_string(resolved.value())};
        curr_bb.push_instr(copy{destination, value});
        return destination;
    }

    value_t generator::process_expr(const ast::logical_expr &expr) {
        const std::string short_circuit_label = get_label("short_circuit");
        const std::string end_label = get_label("logical_end");

        value_t left = process_expr(expr.left);
        value_t result{generate_tmp()};

        if (expr.op == token_t::LogicalAnd) {
            curr_bb.push_instr(jump_if_zero{left, short_circuit_label});

            value_t right = process_expr(expr.right);
            curr_bb.push_instr(copy{result, right});
            curr_bb.push_instr(jump{end_label});

            basic_block_t short_circuit{short_circuit_label};
            bbs.push_back(curr_bb);
            curr_bb = short_circuit;
            curr_bb.push_instr(copy{result, value_t(0)});
            bbs.push_back(std::move(curr_bb));

            curr_bb = basic_block_t{end_label};
        } else if (expr.op == token_t::LogicalOr) {
            curr_bb.push_instr(jump_if_not_zero{left, short_circuit_label});

            const value_t right = process_expr(expr.right);
            curr_bb.push_instr(copy{result, right});
            curr_bb.push_instr(jump{end_label});

            basic_block_t short_circuit{short_circuit_label};
            bbs.push_back(curr_bb);
            curr_bb = short_circuit;

            curr_bb.push_instr(copy{result, value_t(1)});
            bbs.push_back(std::move(curr_bb));

            curr_bb = basic_block_t{end_label};
        }

        return result;
    }

    value_t generator::process_expr(const ast::call_expr &call) {
        std::vector<value_t> arg_values;
        for (const auto &arg: call.arguments) {
            arg_values.push_back(process_expr(arg));
        }

        value_t result{generate_tmp()};

        curr_bb.push_instr(func_call{call.identifier, arg_values, result});

        return result;
    }

    void generator::process_stmt(const ast::stmt::function_param &stmt) {
        throw std::runtime_error("Not implemented\n");
    }

    void generator::process_stmt(const ast::stmt::function_decl &func) {
        resolver.begin_scope();

        for (const auto &param: func.params) {
            resolver.declare(param.name);
        }

        if (!curr_bb.empty()) {
            bbs.push_back(curr_bb);
        }

        curr_bb = basic_block_t(func.function_name + "_entry");
        process_stmt(func.body);

        bbs.push_back(curr_bb);
        resolver.end_scope();
        curr_bb = basic_block_t("entry");
    }

    void generator::process_stmt(const ast::stmt::variable &variable) {
        const auto scope_id = resolver.declare(variable.name);

        if (variable.initializer.has_value()) {
            const auto rhs = process_expr(variable.initializer.value());
            const auto lhs = value_t{variable.name + "_" + std::to_string(scope_id.value())};
            curr_bb.push_instr(copy{lhs, rhs});
        } else {
            throw std::runtime_error("Not implemented?");
            // current_block.add_instruction(ir_value{stmt.name}):
        }
    }
}
