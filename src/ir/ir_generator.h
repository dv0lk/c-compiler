#pragma once
#include <print>
#include <vector>
#include "ir.h"
#include "parser/ast.h"
#include "scope/resolver.hpp"

namespace compiler::ir {
    class ir_generator {
    public:
        std::vector<ir_basic_block> generate(const std::vector<ast::stmt_ptr>& ast);

    private:
        std::vector<ir_basic_block> blocks;
        ir_basic_block current_block{"entry"};
        Resolver resolver;
        int temp_var_counter = 0;

        std::string generate_temp();

        std::string get_label(const std::string& label);

        void process_stmt(const ast::stmt_ptr& stmt_var);

        ir_value process_expr(const ast::expr& expr_var);

        void process_stmt(const ast::return_stmt& ret);

        void process_stmt(const ast::expression_stmt& stmt);

        void process_stmt(const ast::block_stmt& block);

        void process_stmt(const ast::if_stmt& stmt);

        void process_stmt(const ast::while_stmt& stmt);


        ir_value process_expr(const ast::literal_expr& literal);

        ir_value process_expr(const ast::variable_expr& variable);

        ir_value process_expr(const ast::binary_expr& expr);

        ir_value process_expr(const ast::unary_expr& expr);

        ir_value process_expr(const ast::grouping_expr& expr);

        ir_value process_expr(const ast::assignment_expr& expr);

        ir_value process_expr(const ast::logical_expr& expr);

        ir_value process_expr(const ast::call_expr& call);

        void process_stmt(const ast::function_param_stmt& stmt);

        void process_stmt(const ast::function_decl_stmt& func);

        void process_stmt(const ast::variable_stmt& variable);
    };
}
