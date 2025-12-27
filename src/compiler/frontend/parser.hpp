#pragma once
#include "ast/ast.hpp"
#include <vector>

namespace compiler::ast {
    class Parser {
    public:
        [[nodiscard]] std::vector<stmt::stmt_ptr> parse_ast(std::vector<Token>& tokens);

        [[nodiscard]] static std::vector<stmt::stmt_ptr> get_ast(std::vector<Token>& tokens) {
            Parser p;
            return p.parse_ast(tokens);
        }

    private:
        std::vector<Token> tokens;
        std::vector<stmt::stmt_ptr> statements;
        int current_position = 0;

        [[nodiscard]] bool is_end() const;

        bool check_and_advance(const TokenType type) {
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

        Token advance();

        [[nodiscard]] Token peek() const;

        [[nodiscard]] std::optional<Token> peek_next() const;

        [[nodiscard]] Token previous() const;

        [[nodiscard]] bool check(TokenType type) const;

        Token consume(TokenType type, const std::string& error_message);

        expr::expr_ptr parse_expression();

        expr::expr_ptr parse_assignment();

        expr::expr_ptr parse_logical_or();

        expr::expr_ptr parse_logical_and();

        expr::expr_ptr parse_equality();

        expr::expr_ptr parse_comparison();

        expr::expr_ptr parse_additive();

        expr::expr_ptr parse_multiplicative();

        expr::expr_ptr parse_unary();

        expr::expr_ptr parse_primary_expr();

        expr::expr_ptr parse_call(const std::string& name);

        stmt::stmt_ptr parse_statement();

        stmt::stmt_ptr parse_if();

        stmt::stmt_ptr parse_block();

        stmt::stmt_ptr parse_while();

        stmt::stmt_ptr parse_expression_stmt();

        stmt::stmt_ptr parse_return();

        stmt::stmt_ptr parse_var_declaration();

        stmt::stmt_ptr parse_variable_declaration();

        stmt::stmt_ptr parse_function_declaration();
    };
} // namespace compiler::ast
