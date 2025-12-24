#include "parser.hpp"
#include <stdexcept>


namespace compiler::ast {
    std::vector<stmt::stmt_ptr> Parser::parse_ast(std::vector<Token> &tokens) {
        //TODO fix this
        this->tokens = tokens;
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

    bool Parser::check(const token_t type) const {
        if (is_end()) {
            return false;
        }
        return peek().get_type() == type;
    }

    Token Parser::consume(const token_t type, const std::string &error_message) {
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
        if (match(token_t::Equal)) {
            auto value = parse_assignment();

            if (const auto variable = expression->get_if<expr::variable>()) {
                auto name = variable->name;
                return expr::make_expr<expr::assignment>(name, value);
            }
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_logical_or() {
        auto expression = parse_logical_and();
        while (match(token_t::LogicalOr)) {
            auto op = previous().get_type();
            auto right = parse_logical_and();
            expression = expr::make_expr<expr::logical>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_logical_and() {
        auto expression = parse_equality();

        while (match(token_t::LogicalAnd)) {
            auto op = previous().get_type();
            auto right = parse_equality();
            expression = expr::make_expr<expr::logical>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_equality() {
        auto expression = parse_comparison();
        while (match(token_t::NotEqual, token_t::EqualEqual)) {
            auto op = previous().get_type();
            auto right = parse_comparison();
            expression = expr::make_expr<expr::binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_comparison() {
        auto expression = parse_additive();
        while (match(token_t::Less, token_t::LessEqual, token_t::Greater, token_t::GreaterEqual)) {
            auto op = previous().get_type();
            auto right = parse_additive();
            expression = expr::make_expr<expr::binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_additive() {
        auto expression = parse_multiplicative();
        while (match(token_t::Plus, token_t::Minus)) {
            auto op = previous().get_type();
            auto right = parse_multiplicative();
            expression = expr::make_expr<expr::binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_multiplicative() {
        auto expression = parse_unary();
        while (match(token_t::Star, token_t::Slash)) {
            auto op = previous().get_type();
            auto right = parse_unary();
            expression = expr::make_expr<expr::binary>(expression, op, right);
        }
        return expression;
    }

    expr::expr_ptr Parser::parse_unary() {
        if (match(token_t::Tilde, token_t::Minus, token_t::Not)) {
            auto op = previous().get_type();
            auto right = parse_unary();
            return expr::make_expr<expr::unary>(op, right);
        }
        return parse_primary_expr();
    }

    expr::expr_ptr Parser::parse_primary_expr() {
        if (match(token_t::IntLiteral, token_t::StringLiteral, token_t::DoubleLiteral)) {
            return expr::make_expr<expr::literal>(*previous().get_literal());
        }

        if (match(token_t::LeftParen)) {
            auto expr = parse_expression();
            consume(token_t::RightParen, "Expected ')' after expression");
            return expr::make_expr<expr::grouping>(expr);
        }

        if (match(token_t::Identifier)) {
            const std::string name = previous().get_lexeme();
            if (match(token_t::LeftParen))
                return parse_call(name);
            return expr::make_expr<expr::variable>(name);
        }

        throw std::runtime_error("Encounter Unknown expression while parsing");
    }

    expr::expr_ptr Parser::parse_call(const std::string &name) {
        std::vector<expr::expr_ptr> arguments;

        if (!check(token_t::RightParen)) {
            do {
                arguments.push_back(parse_expression());
            } while (match(token_t::Comma));
        }

        consume(token_t::RightParen, "Expected ')' after arguments");
        return expr::make_expr<expr::call>(name, arguments);
    }

    stmt::stmt_ptr Parser::parse_statement() {
        if (match(token_t::If)) {
            return parse_if();
        }

        if (match(token_t::While)) {
            return parse_while();
        }

        if (match(token_t::LeftBrace)) {
            return parse_block();
        }

        if (match(token_t::Return)) {
            return parse_return();
        }

        return parse_expression_stmt();
    }

    stmt::stmt_ptr Parser::parse_var_declaration() {
        if (match(token_t::Int, token_t::Char, token_t::Void, token_t::Double)) {
            auto next_token = peek_next();

            if (next_token.has_value()) {
                if (next_token->get_type() == token_t::Equal || next_token->get_type() == token_t::Semicolon) {
                    return parse_variable_declaration();
                }

                if (next_token->get_type() == token_t::LeftParen) {
                    return parse_function_declaration();
                }
            }
            throw std::runtime_error("Expected variable declaration or function declaration");
        }
        return parse_statement();
    }

    //todo add support for multiple types
    stmt::stmt_ptr Parser::parse_variable_declaration() {
        std::string variable_name = consume(token_t::Identifier, "Expected identifier after type").get_lexeme();

        std::optional<expr::expr_ptr> initializer;
        if (match(token_t::Equal)) {
            initializer = parse_expression();
        }
        consume(token_t::Semicolon, "Expected ';' after variable declaration");

        return stmt::make_stmt<stmt::variable>(variable_name, initializer);
    }

    stmt::stmt_ptr Parser::parse_function_declaration() {
        auto return_type = previous().get_type();
        auto function_name = consume(token_t::Identifier, "Expected function name after type").get_lexeme();
        consume(token_t::LeftParen, "Expected '(' after function name");

        std::vector<stmt::function_param> params;
        if (!check(token_t::RightParen)) {
            do {
                //todo currently support only int
                auto param_type = consume(token_t::Int, "Expected parameter type").get_type();
                auto param_name = consume(token_t::Identifier, "Expected parameter name").get_lexeme();
                params.emplace_back(param_name, param_type);
            } while (match(token_t::Comma) && !is_end());
        }

        consume(token_t::RightParen, "Expected ')' after parameters");
        consume(token_t::LeftBrace, "Expected '{' before function body");

        auto body = parse_block();

        return stmt::make_stmt<stmt::function_decl>(return_type, function_name, params, body);
    }

    stmt::stmt_ptr Parser::parse_if() {
        consume(token_t::LeftParen, "Expected '(' after if");
        auto condition = parse_expression();
        consume(token_t::RightParen, "Expected ')' after if condition");
        auto then_branch = parse_statement();

        std::optional<stmt::stmt_ptr> else_branch;
        if (match(token_t::Else)) {
            else_branch = parse_statement();
        }

        return stmt::make_stmt<stmt::if_>(condition, then_branch, else_branch);
    }

    stmt::stmt_ptr Parser::parse_block() {
        std::vector<stmt::stmt_ptr> statements;

        while (!check(token_t::RightBrace) && !is_end()) {
            statements.emplace_back(parse_var_declaration());
        }
        consume(token_t::RightBrace, "Expected '}' after block");

        return stmt::make_stmt<stmt::block>(statements);
    }

    stmt::stmt_ptr Parser::parse_while() {
        consume(token_t::LeftParen, "Expected '(' after while");
        auto condition = parse_expression();
        consume(token_t::RightParen, "Expected ')' after while condition");
        auto body = parse_statement();
        return stmt::make_stmt<stmt::while_>(condition, body);
    }

    stmt::stmt_ptr Parser::parse_expression_stmt() {
        auto expression = stmt::make_stmt<stmt::expression>(parse_expression());
        consume(token_t::Semicolon, "Expected ';' after expression");
        return expression;
    }

    stmt::stmt_ptr Parser::parse_return() {
        auto expression = parse_expression();
        auto return_stmt = stmt::make_stmt<stmt::return_>(expression);
        consume(token_t::Semicolon, "Expected ';' after return statement");
        return return_stmt;
    }
}
