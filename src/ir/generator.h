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
        Resolver resolver_;
        std::vector<basic_block_t> blocks_;
        basic_block_t current_block_{"entry"};

        int var_counter_ = 0;
        int label_counter_ = 0;

        //push current block to blocks
        void finalize_current_block();
        //finalize current block and start new block with provided label
        void start_new_bb(std::string label);

        std::string make_tmp_var();
        std::string make_label(const std::string &label);
        std::string make_variable_name(const std::string& name, size_t scope_id);

        void emit_stmt(const ast::stmt::stmt_ptr &stmt_var);

        void emit_stmt(const ast::stmt::return_ &ret);

        void emit_stmt(const ast::stmt::expression &stmt);

        void emit_stmt(const ast::stmt::block &block);

        void emit_stmt(const ast::stmt::if_ &stmt);

        void emit_stmt(const ast::stmt::while_ &stmt);

        void emit_stmt(const ast::stmt::function_param &stmt);

        void emit_stmt(const ast::stmt::function_decl &func);

        void emit_stmt(const ast::stmt::variable &variable);

        value_t emit_expr(const ast::expr::expr_ptr &expr_var);

        value_t emit_expr(const ast::expr::literal &literal);

        value_t emit_expr(const ast::expr::variable &variable);

        value_t emit_expr(const ast::expr::binary &expr);

        value_t emit_expr(const ast::expr::unary &expr);

        value_t emit_expr(const ast::expr::grouping &expr);

        value_t emit_expr(const ast::expr::assignment &expr);

        value_t emit_expr(const ast::expr::logical &expr);

        value_t emit_expr(const ast::expr::call &expr);
    };
}
