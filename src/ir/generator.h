#pragma once
#include <print>
#include <vector>

#include "types/basic_block_t.hpp"

#include "scope/resolver.hpp"
#include "ir.hpp"
#include "parser/ast.hpp"
// #include "../parser/ast.hpp"

namespace compiler::ir {
    class generator {
    public:
        std::vector<basic_block_t> generate(const std::vector<ast::stmt::stmt_ptr> &ast);

    private:
        std::vector<basic_block_t> bbs;
        basic_block_t curr_bb{"entry"};
        Resolver resolver;
        int temp_var_counter = 0;

        //pushes current block to bbs and starts a new bb
        void start_new_bb(std::string label);

        std::string generate_tmp();

        static std::string get_label(const std::string &label);

        void process_stmt(const ast::stmt::stmt_ptr &stmt_var);

        // void process_stmt(const ast::stmt &stmt_var);

        value_t process_expr(const ast::expr_ptr &expr_var);

        // ir_value process_expr(const ast::expr &expr_var);

        void process_stmt(const ast::stmt::return_ &ret);

        void process_stmt(const ast::stmt::expression &stmt);

        void process_stmt(const ast::stmt::block &block);

        void process_stmt(const ast::stmt::if_ &stmt);

        void process_stmt(const ast::stmt::while_ &stmt);

        value_t process_expr(const ast::literal_expr &literal);

        value_t process_expr(const ast::variable_expr &variable);

        value_t process_expr(const ast::binary_expr &expr);

        value_t process_expr(const ast::unary_expr &expr);

        value_t process_expr(const ast::grouping_expr &expr);

        value_t process_expr(const ast::assignment_expr &expr);

        value_t process_expr(const ast::logical_expr &expr);

        value_t process_expr(const ast::call_expr &call);

        void process_stmt(const ast::stmt::function_param &stmt);

        void process_stmt(const ast::stmt::function_decl &func);

        void process_stmt(const ast::stmt::variable &variable);
    };
}
