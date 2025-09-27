#pragma once
#include <vector>

#include "ast.h"

#include "lexer/lexer.h"


namespace compiler::parser {
    class parser {
    public:
        [[nodiscard]] std::vector<ast::stmt_ptr> parse_ast(std::vector<token> tokens);

    private:
        std::vector<token> tokens;
        std::vector<ast::stmt_ptr> statements;
        int current_position = 0;

        [[nodiscard]] bool is_end() const;

        bool check_and_advance(const token_type type) {
            if (peek().get_type() == type) {
                advance();
                return true;
            }
            return false;
        }

        template <typename... Types>
        bool match(Types... types) {
            return (check_and_advance(types) || ...);
        }

        token advance();

        [[nodiscard]] token peek() const;

        [[nodiscard]] std::optional<token> peek_next() const;

        [[nodiscard]] token previous() const;

        [[nodiscard]] bool check(token_type type) const;

        token consume(token_type type, const std::string& error_message);


        //Statements
        ast::stmt_ptr parse_statement();

        ast::stmt_ptr parse_if_statement();

        ast::stmt_ptr parse_block_statement();

        ast::stmt_ptr parse_while_statement();

        ast::stmt_ptr parse_expression_statement();

        ast::stmt_ptr parse_return_statement();

        ast::stmt_ptr parse_declaration_statement();

        ast::stmt_ptr parse_variable_declaration_statement();

        ast::stmt_ptr parse_function_declaration_statement();

        //Expressions
        ast::expr_ptr parse_expression();

        ast::expr_ptr parse_assignment_expr();

        ast::expr_ptr parse_logical_or_expr();

        ast::expr_ptr parse_logical_and_expr();

        ast::expr_ptr parse_equality_expr();

        ast::expr_ptr parse_comparison_expr();

        ast::expr_ptr parse_additive_expr();

        ast::expr_ptr parse_multiplicative_expr();

        ast::expr_ptr parse_unary_expr();

        ast::expr_ptr parse_primary_expr();

        ast::expr_ptr parse_call_expr(const std::string& name);
    };
}

