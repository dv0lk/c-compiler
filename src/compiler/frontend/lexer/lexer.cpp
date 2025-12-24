#include "lexer.hpp"

#include <iostream>
#include <stdexcept>

namespace compiler::lexer {
    std::vector<Token> tokenizer::parse_tokens(const std::string& source) {
        this->source = source;

        while (!is_end()) {
            start_position = current_position;
            lex();
        }
        return this->tokens;
    }

    void tokenizer::print_tokens() const {
        if (this->tokens.empty()) {
            std::cout << "No tokens to print\n";
            return;
        }
        std::cout << "-------\n";
        std::cout << "Tokens:\n";

        for (const auto& token : this->tokens) {
            std::cout << token.type_to_string() << " ";
        }
    }

    void tokenizer::lex() {
        const char c = peek();
        advance();

        switch (c) {
        case '(':
            add_token(TokenType::LeftParen);
            break;
        case ')':
            add_token(TokenType::RightParen);
            break;
        case '{':
            add_token(TokenType::LeftBrace);
            break;
        case '}':
            add_token(TokenType::RightBrace);
            break;
        case ',':
            add_token(TokenType::Comma);
            break;
        case '.':
            add_token(TokenType::Dot);
            break;
        case '?':
            add_token(TokenType::QuestionMark);
            break;
        case ';':
            add_token(TokenType::Semicolon);
            break;
        case '*':
            add_token(TokenType::Star);
            break;
        case '/':
            if (match_next('/')) {
                skip_line();
            } else if (match_next('*')) {
                skip_multiline_comment();
            } else {
                add_token(TokenType::Slash);
            }
            break;
        case '!':
            add_token(match_next('=') ? TokenType::NotEqual : TokenType::Not);
            break;
        case '=':
            add_token(match_next('=') ? TokenType::EqualEqual : TokenType::Equal);
            break;
        case '<':
            add_token(match_next('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            add_token(match_next('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
        case '+':
            add_token(match_next('+') ? TokenType::PlusPlus : TokenType::Plus);
            break;
        case '-':
            add_token(match_next('-') ? TokenType::MinusMinus : TokenType::Minus);
            break;
        case '&':
            add_token(match_next('&') ? TokenType::LogicalAnd : TokenType::Ampersand);
            break;
        case '|':
            add_token(match_next('|') ? TokenType::LogicalOr : TokenType::Pipe);
            break;
        case '~':
            add_token(TokenType::Tilde);
            break;
        case '"':
            consume_string();
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            break;
        default:
            if (isdigit(c)) {
                consume_digit();
            } else if (is_alpha(c)) {
                consume_identifier();
            } else {
                throw std::runtime_error("Unknown symbol while lexing");
            }
        }
    }

    bool tokenizer::is_end() const {
        return current_position >= source.size();
    }

    bool tokenizer::is_alpha(const char c) {
        return std::isalpha(c) || c == '_';
    }

    char tokenizer::peek() const {
        return source[current_position];
    }

    char tokenizer::peek_next() const {
        if (is_end()) {
            return '\0';
        }
        return source[current_position + 1];
    }

    bool tokenizer::match_next(const char c) {
        const bool match = peek() == c;
        if (match) {
            advance();
        }
        return match;
    }

    void tokenizer::advance() {
        current_position++;
    }

    std::string tokenizer::get_lexeme() const {
        return source.substr(start_position, current_position - start_position);
    }

    void tokenizer::add_token(TokenType type, std::optional<int> literal) {
        tokens.emplace_back(type, get_lexeme(), literal);
    }


    //todo handle stirng literals
    void tokenizer::consume_string() {
        while (peek() != '"' && !is_end()) {
            //todo maybe think about adding line support
            // if (peek() == '\n') {
            //     line++;
            // }
            advance();
        }

        if (is_end()) {
            throw std::runtime_error("Unterminated string\n");
        }

        //todo huh, something is off here, works for now
        const std::string lexeme = source.substr(start_position, current_position - start_position);

        add_token(TokenType::StringLiteral);
    }

    //todo handle double/float values
    void tokenizer::consume_digit() {
        bool is_decimal = false;

        while (isdigit(peek())) {
            advance();
        }

        if (peek() == '.') {
            // throw std::runtime_error("double/float value are currently unsupported");
            is_decimal = true;
            advance();

            while (isdigit(peek())) {
                advance();
            }
        }

        const auto string_value = source.substr(start_position, current_position - start_position);

        if (is_decimal) {
            throw std::runtime_error("double/float values are currently unsupported");
            // auto value = std::stod(string_value);
            // add_token(token_type::DoubleLiteral, value);
        }

        int value = std::stoi(string_value);
        add_token(TokenType::IntLiteral, value);
    }

    void tokenizer::consume_identifier() {
        while (is_alpha(peek()) || isdigit(peek())) {
            advance();
        }

        if (is_end()) {
            throw std::runtime_error("Unterminated identifier\n");
        }

        const auto value = source.substr(start_position, current_position - start_position);

        const auto type = keyword_or_identifier(value);

        add_token(type);
    }

    void tokenizer::skip_line() {
        while (peek() != '\n' && !is_end()) {
            advance();
        }
    }

    void tokenizer::skip_multiline_comment() {
        while (!is_end()) {
            if (peek() == '*' && peek_next() == '/') {
                advance();
                advance();
                break;
            }
            // if (peek() == '\n') {
            //     line++;
            // }
            advance();
        }
    }

    TokenType tokenizer::keyword_or_identifier(const std::string& str) {
        static const std::unordered_map<std::string_view, TokenType> keywords = {
            {"break", TokenType::Break},
            {"continue", TokenType::Continue},
            {"do", TokenType::Do},
            {"else", TokenType::Else},
            {"false", TokenType::False},
            {"for", TokenType::For},
            {"if", TokenType::If},
            {"return", TokenType::Return},
            {"true", TokenType::True},
            {"while", TokenType::While},
            {"continue", TokenType::Continue},
            {"else", TokenType::Else},
            {"false", TokenType::False},
            {"for", TokenType::For},
            {"if", TokenType::If},
            {"return", TokenType::Return},
            {"true", TokenType::True},
            {"while", TokenType::While},
            {"int", TokenType::Int},
            {"void", TokenType::Void},
            {"float", TokenType::Float},
            {"char", TokenType::Char},
            {"bool", TokenType::Bool},
            {"goto", TokenType::Goto},
            {"struct", TokenType::Struct},
        };

        if (keywords.contains(str)) {
            return keywords.at(str);
        }

        return TokenType::Identifier;
    }
}
