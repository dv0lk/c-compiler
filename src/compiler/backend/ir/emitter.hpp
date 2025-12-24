#pragma once
#include <print>
#include <vector>

#include "structure/program.hpp"
#include "types/instruction.hpp"
#include "misc/resolver.hpp"
#include "frontend/ast/ast.hpp"

namespace compiler::ir {
    class Emitter {
    public:
        using program_t = Program<Instruction>;
        using function_t = Function<Instruction>;
        using bb_t = BasicBlock<Instruction>;

        Program<Instruction> emit(const std::vector<ast::stmt::stmt_ptr> &ast);

        [[nodiscard]] static program_t get_ir(const std::vector<ast::stmt::stmt_ptr> &ast) {
            Emitter e;
            return e.emit(ast);
        }

    private:
        program_t program_;
        function_t current_function_;
        bb_t current_bb_;

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

        Operand emit_expr(const ast::expr::expr_ptr &expr_var);

        Operand emit_expr(const ast::expr::literal &literal);

        Operand emit_expr(const ast::expr::variable &variable);

        Operand emit_expr(const ast::expr::binary &expr);

        Operand emit_expr(const ast::expr::unary &expr);

        Operand emit_expr(const ast::expr::grouping &expr);

        Operand emit_expr(const ast::expr::assignment &expr);

        Operand emit_expr(const ast::expr::logical &expr);

        Operand emit_expr(const ast::expr::call &expr);
    };
}
