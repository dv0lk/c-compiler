#include "ir_generator.h"

//TODO start_new_block
namespace compiler::ir {
    std::vector<ir_basic_block> ir_generator::generate(const std::vector<ast::stmt_ptr>& ast) {
        current_block = ir_basic_block("entry");

        for (const auto& stmt : ast) {
            process_stmt(stmt);
        }

        if (!current_block.is_empty())
            blocks.push_back(current_block);

        return blocks;
    }

    std::string ir_generator::generate_temp() {
        return "t" + std::to_string(temp_var_counter++);
    }

    std::string ir_generator::get_label(const std::string& label) {
        static int count = 0;
        return label + "_" + std::to_string(count++);
    }

    void ir_generator::process_stmt(const ast::stmt_ptr& stmt_var) {
        // std::cout << "Processing stmt var\n";
        std::visit([this](const auto& stmt) {
            this->process_stmt(stmt);
        }, *stmt_var);
    }

    ir_value ir_generator::process_expr(const ast::expr& expr_var) {
        // std::cout << "Processing expr var\n";
        return std::visit([this](const auto& expr) {
            return this->process_expr(expr);
        }, expr_var);
    }

    void ir_generator::process_stmt(const ast::return_stmt& ret) {
        // std::cout << "Processing return\n";
        const ir_value return_value = process_expr(*ret.value);
        current_block.add_instruction(ir_return{return_value});
    }

    void ir_generator::process_stmt(const ast::expression_stmt& stmt) {
        process_expr(*stmt.expr);
    }

    void ir_generator::process_stmt(const ast::block_stmt& block) {
        resolver.begin_scope();

        if (block.statements.empty()) {
            throw std::runtime_error("Empty block?");

        }

        for (const auto& s : block.statements) {
            process_stmt(s);
        }

        resolver.end_scope();
    }

    void ir_generator::process_stmt(const ast::if_stmt& stmt) {
        const std::string else_label = get_label("else");
        const std::string end_label = get_label("end");
        const ir_value condition = process_expr(*stmt.condition);
        current_block.add_instruction(ir_jump_if_zero{condition, else_label});

        process_stmt(stmt.then_branch);

        if (stmt.else_branch.has_value()) {
            current_block.add_instruction(ir_jump{end_label});
            current_block.add_instruction(ir_label{else_label});
            process_stmt(*stmt.else_branch);
            current_block.add_instruction(ir_label{end_label});
        } else {
            current_block.add_instruction(ir_label{else_label});
        }
    }

    void ir_generator::process_stmt(const ast::while_stmt& stmt) {
        const std::string cond_label = get_label("while_cond");
        const std::string body_label = get_label("while_body");
        const std::string end_label = get_label("while_end");

        current_block.add_instruction(ir_jump{cond_label});
        blocks.push_back(current_block);

        current_block = ir_basic_block{cond_label};
        const ir_value condition = process_expr(*stmt.condition);
        current_block.add_instruction(ir_jump_if_zero{condition, end_label});
        current_block.add_instruction(ir_jump{body_label});
        blocks.push_back(current_block);

        current_block = ir_basic_block{body_label};
        process_stmt(stmt.body);
        current_block.add_instruction(ir_jump{cond_label});
        blocks.push_back(current_block);

        current_block = ir_basic_block{end_label};
    }

    ir_value ir_generator::process_expr(const ast::literal_expr& literal) {
        return ir_value(literal.value);
    }

    ir_value ir_generator::process_expr(const ast::variable_expr& variable) {
        const auto resolved = resolver.resolve(variable.name);

        if (!resolved.has_value())
            throw std::runtime_error("Error resolving variable\n");

        const std::string name = variable.name + "_" + std::to_string(resolved.value());
        return ir_value(name);
    }

    ir_value ir_generator::process_expr(const ast::binary_expr& expr) {
        const ir_value left = process_expr(*expr.left);
        const ir_value right = process_expr(*expr.right);
        ir_value result{generate_temp()};

        current_block.add_instruction(ir_binary{expr.op, left, right, result});
        return result;
    }

    ir_value ir_generator::process_expr(const ast::unary_expr& expr) {
        const ir_value operand = process_expr(*expr.value);
        ir_value result{generate_temp()};

        current_block.add_instruction(ir_unary{expr.op, operand, result});
        return result;
    }

    ir_value ir_generator::process_expr(const ast::grouping_expr& expr) {
        return process_expr(*expr.expr);
    }

    ir_value ir_generator::process_expr(const ast::assignment_expr& expr) {
        const ir_value value = process_expr(*expr.value);
        const auto resolved = resolver.resolve(expr.name);
        if (!resolved.has_value())
            throw std::runtime_error("Undefined variable assignment");

        ir_value destination{expr.name + "_" + std::to_string(resolved.value())};
        current_block.add_instruction(ir_copy{destination, value});
        return destination;
    }

    ir_value ir_generator::process_expr(const ast::logical_expr& expr) {
        const std::string short_circuit_label = get_label("short_circuit");
        const std::string end_label = get_label("logical_end");

        ir_value left = process_expr(*expr.left);
        ir_value result{generate_temp()};

        if (expr.op == token_type::LogicalAnd) {
            current_block.add_instruction(ir_jump_if_zero{left, short_circuit_label});

            ir_value right = process_expr(*expr.right);
            current_block.add_instruction(ir_copy{result, right});
            current_block.add_instruction(ir_jump{end_label});

            ir_basic_block short_circuit{short_circuit_label};
            blocks.push_back(current_block);
            current_block = short_circuit;
            current_block.add_instruction(ir_copy{result, ir_value(0)});
            blocks.push_back(std::move(current_block));

            current_block = ir_basic_block{end_label};
        } else if (expr.op == token_type::LogicalOr) {
            current_block.add_instruction(ir_jump_if_not_zero{left, short_circuit_label});

            const ir_value right = process_expr(*expr.right);
            current_block.add_instruction(ir_copy{result, right});
            current_block.add_instruction(ir_jump{end_label});

            ir_basic_block short_circuit{short_circuit_label};
            blocks.push_back(current_block);
            current_block = short_circuit;

            current_block.add_instruction(ir_copy{result, ir_value(1)});
            blocks.push_back(std::move(current_block));

            current_block = ir_basic_block{end_label};
        }

        return result;
    }

    ir_value ir_generator::process_expr(const ast::call_expr& call) {
        std::vector<ir_value> arg_values;
        for (const auto& arg : call.arguments) {
            arg_values.push_back(process_expr(*arg));
        }

        ir_value result {generate_temp()};

        current_block.add_instruction(ir_call{call.identifier, arg_values, result});

        return result;
    }

    void ir_generator::process_stmt(const ast::function_param_stmt& stmt) {
        throw std::runtime_error("Not implemented\n");
    }

    void ir_generator::process_stmt(const ast::function_decl_stmt& func) {
        resolver.begin_scope();

        for (const auto& param : func.params) {
            resolver.declare(param.name);
        }

        if (!current_block.is_empty()) {
            blocks.push_back(current_block);
        }

        current_block = ir_basic_block(func.function_name + "_entry");
        process_stmt(func.body);

        blocks.push_back(current_block);
        resolver.end_scope();
        current_block = ir_basic_block("entry");
    }

    void ir_generator::process_stmt(const ast::variable_stmt& variable) {
        const auto scope_id = resolver.declare(variable.name);

        if (variable.initializer.has_value()) {
            const auto rhs = process_expr(**variable.initializer);
            const auto lhs = ir_value{variable.name + "_" + std::to_string(scope_id.value())};
            current_block.add_instruction(ir_copy{lhs, rhs});
        } else {
            throw std::runtime_error("Not implemented?");
            // current_block.add_instruction(ir_value{stmt.name}):
        }
    }
}
