#pragma once
#include <vector>

#include "ast.hpp"

#include "lexer/lexer.h"


namespace compiler::ast {
    class parser {
    public:
        [[nodiscard]] std::vector<stmt::stmt_ptr> parse_ast(std::vector<token> &tokens);

    private:
        std::vector<token> tokens;
        std::vector<stmt::stmt_ptr> statements;
        int current_position = 0;

        [[nodiscard]] bool is_end() const;

        bool check_and_advance(const token_t type) {
            if (peek().get_type() == type) {
                advance();
                return true;
            }
            return false;
        }

        template<typename... Types>
        bool match(Types... types) {
            return (check_and_advance(types) || ...);
        }

        token advance();

        [[nodiscard]] token peek() const;

        [[nodiscard]] std::optional<token> peek_next() const;

        [[nodiscard]] token previous() const;

        [[nodiscard]] bool check(token_t type) const;

        token consume(token_t type, const std::string &error_message);

        expr_ptr parse_expression();

        expr_ptr parse_assignment_expr();

        expr_ptr parse_logical_or_expr();

        expr_ptr parse_logical_and_expr();

        expr_ptr parse_equality_expr();

        expr_ptr parse_comparison_expr();

        expr_ptr parse_additive_expr();

        expr_ptr parse_multiplicative_expr();

        expr_ptr parse_unary_expr();

        expr_ptr parse_primary_expr();

        expr_ptr parse_call_expr(const std::string &name);


        stmt::stmt_ptr parse_statement();

        stmt::stmt_ptr parse_if_statement();

        stmt::stmt_ptr parse_block_statement();

        stmt::stmt_ptr parse_while_statement();

        stmt::stmt_ptr parse_expression_statement();

        stmt::stmt_ptr parse_return_statement();

        stmt::stmt_ptr parse_declaration_statement();

        stmt::stmt_ptr parse_variable_declaration_statement();

        stmt::stmt_ptr parse_function_declaration_statement();
    };
}

