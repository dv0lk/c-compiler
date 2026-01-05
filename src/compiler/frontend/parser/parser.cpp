#include "parser.hpp"

#include <print>
#include <stdexcept>

namespace compiler::ast {
    std::vector<stmt::stmt_ptr> Parser::parse_ast(const std::span<Token> input_tokens) {
        this->tokens = input_tokens;
        while (!is_end()) {
            this->statements.emplace_back(parse_var_declaration());
        }
        return this->statements;
    }

    bool Parser::is_end() const {
        return current_position >= tokens.size();
    }

    Token Parser::advance() {
        current_position++;
        return previous();
    }

    Token Parser::peek() const {
        return tokens[current_position];
    }

    std::optional<Token> Parser::peek_next() const {
        if (is_end()) {
            return std::nullopt;
        }
        return tokens[current_position + 1];
    }

    Token Parser::previous() const {
        return tokens[current_position - 1];
    }

    bool Parser::check(const TokenType type) const {
        if (is_end()) {
            return false;
        }
        return peek().get_type() == type;
    }

    Token Parser::consume(const TokenType type, const std::string& error_message) {
        if (check(type)) {
            return advance();
        }
        throw std::runtime_error(error_message);
    }

    expr::expr_ptr Parser::parse_expression() {
        return parse_assignment();
    }

    expr::expr_ptr Parser::parse_assignment() {
        auto expression = parse_logical_or();
        if (match(TokenType::Equal)) {
            auto value = parse_assignment();

            if (const auto variable = expression->get_if<expr::Variable>()) {
                auto name = variable->name;
                return expr::make_expr<expr::Assignment>(name, value);
            }
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_logical_or() {
        auto expression = parse_logical_and();
        while (match(TokenType::LogicalOr)) {
            auto op = previous().get_type();
            auto right = parse_logical_and();
            expression = expr::make_expr<expr::Logical>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_logical_and() {
        auto expression = parse_equality();

        while (match(TokenType::LogicalAnd)) {
            auto op = previous().get_type();
            auto right = parse_equality();
            expression = expr::make_expr<expr::Logical>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_equality() {
        auto expression = parse_comparison();
        while (match(TokenType::NotEqual, TokenType::EqualEqual)) {
            auto op = previous().get_type();
            auto right = parse_comparison();
            expression = expr::make_expr<expr::Binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_comparison() {
        auto expression = parse_additive();
        while (match(TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual)) {
            auto op = previous().get_type();
            auto right = parse_additive();
            expression = expr::make_expr<expr::Binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_additive() {
        auto expression = parse_multiplicative();
        while (match(TokenType::Plus, TokenType::Minus)) {
            auto op = previous().get_type();
            auto right = parse_multiplicative();
            expression = expr::make_expr<expr::Binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_multiplicative() {
        auto expression = parse_unary();
        while (match(TokenType::Star, TokenType::Slash)) {
            auto op = previous().get_type();
            auto right = parse_unary();
            expression = expr::make_expr<expr::Binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_unary() {
        if (match(TokenType::Tilde, TokenType::Minus, TokenType::Not)) {
            auto op = previous().get_type();
            auto right = parse_unary();
            return expr::make_expr<expr::Unary>(op, right);
        }
        return parse_primary_expr();
    }

    expr::expr_ptr Parser::parse_primary_expr() {
        if (match(TokenType::IntLiteral, TokenType::StringLiteral, TokenType::DoubleLiteral)) {
            return expr::make_expr<expr::Literal>(*previous().get_literal());
        }

        if (match(TokenType::LeftParen)) {
            auto expr = parse_expression();
            consume(TokenType::RightParen, "Expected ')' after expression");
            return expr::make_expr<expr::Grouping>(expr);
        }

        if (match(TokenType::Identifier)) {
            const auto name = previous().get_lexeme();
            if (match(TokenType::LeftParen))
                return parse_call(std::string(name));
            return expr::make_expr<expr::Variable>(std::string(name));
        }

        throw std::runtime_error("Encounter Unknown expression while parsing");
    }

    expr::expr_ptr Parser::parse_call(const std::string& name) {
        std::vector<expr::expr_ptr> arguments;

        if (!check(TokenType::RightParen)) {
            do {
                arguments.push_back(parse_expression());
            } while (match(TokenType::Comma));
        }

        consume(TokenType::RightParen, "Expected ')' after arguments");
        return expr::make_expr<expr::Call>(name, arguments);
    }

    stmt::stmt_ptr Parser::parse_statement() {
        if (match(TokenType::If)) {
            return parse_if();
        }

        if (match(TokenType::While)) {
            return parse_while();
        }

        if (match(TokenType::LeftBrace)) {
            return parse_block();
        }

        if (match(TokenType::Return)) {
            return parse_return();
        }

        auto expr = parse_expression();
        consume(TokenType::Semicolon, "Expected ';' after expression");
        return stmt::make_stmt<stmt::ExpressionStmt>(expr);
    }

    stmt::stmt_ptr Parser::parse_var_declaration() {
        if (match(TokenType::Int, TokenType::Char, TokenType::Void, TokenType::Double)) {
            auto next_token = peek_next();

            if (next_token.has_value()) {
                if (next_token->get_type() == TokenType::Equal || next_token->get_type() == TokenType::Semicolon) {
                    return parse_variable_declaration();
                }

                if (next_token->get_type() == TokenType::LeftParen) {
                    return parse_function_declaration();
                }
            }
            throw std::runtime_error("Expected variable declaration or function declaration");
        }
        return parse_statement();
    }

    // todo add support for multiple types
    stmt::stmt_ptr Parser::parse_variable_declaration() {
        std::string variable_name(consume(TokenType::Identifier, "Expected identifier after type").get_lexeme());

        std::optional<expr::expr_ptr> initializer;
        if (match(TokenType::Equal)) {
            initializer = parse_expression();
        }
        consume(TokenType::Semicolon, "Expected ';' after variable declaration");

        return stmt::make_stmt<stmt::Variable>(std::move(variable_name), initializer);
    }

    stmt::stmt_ptr Parser::parse_function_declaration() {
        auto return_type = previous().get_type();
        std::string function_name(consume(TokenType::Identifier, "Expected function name after type").get_lexeme());
        consume(TokenType::LeftParen, "Expected '(' after function name");

        std::vector<stmt::FunctionParam> params;
        if (!check(TokenType::RightParen)) {
            do {
                // todo currently support only int
                auto param_type = consume(TokenType::Int, "Expected parameter type").get_type();
                std::string param_name(consume(TokenType::Identifier, "Expected parameter name").get_lexeme());
                params.emplace_back(std::move(param_name), param_type);
            } while (match(TokenType::Comma) && !is_end());
        }

        consume(TokenType::RightParen, "Expected ')' after parameters");

        // Declaration without body = extern
        if (match(TokenType::Semicolon)) {
            return stmt::make_stmt<stmt::FunctionDecl>(return_type, std::move(function_name), params, std::nullopt);
        }

        consume(TokenType::LeftBrace, "Expected '{' before function body");
        auto body = parse_block();

        return stmt::make_stmt<stmt::FunctionDecl>(return_type, std::move(function_name), params, body);
    }

    stmt::stmt_ptr Parser::parse_if() {
        consume(TokenType::LeftParen, "Expected '(' after if");
        auto condition = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after if condition");
        auto then_branch = parse_statement();

        std::optional<stmt::stmt_ptr> else_branch;
        if (match(TokenType::Else)) {
            else_branch = parse_statement();
        }

        return stmt::make_stmt<stmt::If>(condition, then_branch, else_branch);
    }

    stmt::stmt_ptr Parser::parse_block() {
        std::vector<stmt::stmt_ptr> statements;

        while (!check(TokenType::RightBrace) && !is_end()) {
            statements.emplace_back(parse_var_declaration());
        }
        consume(TokenType::RightBrace, "Expected '}' after block");

        return stmt::make_stmt<stmt::Block>(statements);
    }

    stmt::stmt_ptr Parser::parse_while() {
        consume(TokenType::LeftParen, "Expected '(' after while");
        auto condition = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after while condition");
        auto body = parse_statement();
        return stmt::make_stmt<stmt::While>(condition, body);
    }

    stmt::stmt_ptr Parser::parse_return() {
        auto expression = parse_expression();
        auto return_stmt = stmt::make_stmt<stmt::Return>(expression);
        consume(TokenType::Semicolon, "Expected ';' after return statement");
        return return_stmt;
    }
} // namespace compiler::ast
