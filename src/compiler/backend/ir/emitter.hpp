#pragma once
#include <print>
#include <vector>

#include "core/program.hpp"
#include "frontend/ast/ast.hpp"
#include "resolver.hpp"
#include "types/instruction.hpp"

namespace compiler::ir {
    class Emitter {
    public:
        using program_t = Program<Instruction>;
        using function_t = Function<Instruction>;

        Program<Instruction> emit(const std::vector<ast::stmt::stmt_ptr>& ast);

        [[nodiscard]] static program_t get_ir(const std::vector<ast::stmt::stmt_ptr>& ast) {
            Emitter e;
            return e.emit(ast);
        }

    private:
        program_t program_;
        function_t current_function_;

        Resolver resolver_;

        int var_counter_ = 0;
        int label_counter_ = 0;

        void finalize_current_function();

        void start_new_function(const std::string& function_name);

        std::string make_tmp_var();

        std::string make_label(const std::string& label);

        std::string make_variable_name(const std::string& name, size_t scope_id);

        void emit_stmt(const ast::stmt::stmt_ptr& stmt_var);

        void emit_stmt(const ast::stmt::Return& ret);

        void emit_stmt(const ast::stmt::Block& block);

        void emit_stmt(const ast::stmt::If& stmt);

        void emit_stmt(const ast::stmt::While& stmt);

        void emit_stmt(const ast::stmt::FunctionParam& param);

        void emit_stmt(const ast::stmt::FunctionDecl& func);

        void emit_stmt(const ast::stmt::Variable& variable);

        Operand emit_expr(const ast::expr::expr_ptr& expr_var);

        Operand emit_expr(const ast::expr::Literal& literal);

        Operand emit_expr(const ast::expr::Variable& variable);

        Operand emit_expr(const ast::expr::Binary& expr);

        Operand emit_expr(const ast::expr::Unary& expr);

        Operand emit_expr(const ast::expr::Grouping& expr);

        Operand emit_expr(const ast::expr::Assignment& expr);

        Operand emit_expr(const ast::expr::Logical& expr);

        Operand emit_expr(const ast::expr::Call& expr);
    };
} // namespace compiler::ir
