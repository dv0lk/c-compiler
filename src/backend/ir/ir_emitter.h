#pragma once
#include <print>
#include <vector>

#include "ir/util/resolver.hpp"
#include "types/ir.hpp"
#include "parser/ast.hpp"
#include "base/program.hpp"

namespace compiler::ir {
    class emitter {
    public:
        using program_t = Program<instruction>;
        using function_t = Function<instruction>;
        using bb_t = BasicBlock<instruction>;

        Program<instruction> emit(const std::vector<ast::stmt::stmt_ptr> &ast);

        [[nodiscard]] static program_t get_ir(const std::vector<ast::stmt::stmt_ptr> &ast) {
            emitter e;
            return e.emit(ast);
        }

    private:
        program_t program_;
        function_t current_function_;
        bb_t current_block_;

        Resolver resolver_;

        int var_counter_ = 0;
        int label_counter_ = 0;


        void finalize_current_function();

        void start_new_function(const std::string &function_name);

        //push current block to blocks
        void finalize_current_block();

        //finalize current block and start new block with provided label
        void start_new_bb(const std::string &label);

        std::string make_tmp_var();

        std::string make_label(const std::string &label);

        std::string make_variable_name(const std::string &name, size_t scope_id);

        void emit_stmt(const ast::stmt::stmt_ptr &stmt_var);

        void emit_stmt(const ast::stmt::return_ &ret);

        void emit_stmt(const ast::stmt::expression &stmt);

        void emit_stmt(const ast::stmt::block &block);

        void emit_stmt(const ast::stmt::if_ &stmt);

        void emit_stmt(const ast::stmt::while_ &stmt);

        void emit_stmt(const ast::stmt::function_param &stmt);

        void emit_stmt(const ast::stmt::function_decl &func);

        void emit_stmt(const ast::stmt::variable &variable);

        VirtualReg emit_expr(const ast::expr::expr_ptr &expr_var);

        VirtualReg emit_expr(const ast::expr::literal &literal);

        VirtualReg emit_expr(const ast::expr::variable &variable);

        VirtualReg emit_expr(const ast::expr::binary &expr);

        VirtualReg emit_expr(const ast::expr::unary &expr);

        VirtualReg emit_expr(const ast::expr::grouping &expr);

        VirtualReg emit_expr(const ast::expr::assignment &expr);

        VirtualReg emit_expr(const ast::expr::logical &expr);

        VirtualReg emit_expr(const ast::expr::call &expr);
    };
}
