#include "lexer.h"

#include <iostream>
#include <stdexcept>

namespace compiler::lexer {
    std::vector<token> lexer::parse_tokens(const std::string& input) {
        this->source = input;

        while (!is_end()) {
            start_position = current_position;
            lex();
        }
        return this->tokens;
    }

    void lexer::print_tokens() const {
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

    void lexer::lex() {
        const char c = peek();
        advance();

        switch (c) {
        case '(':
            add_token(token_type::LeftParen);
            break;
        case ')':
            add_token(token_type::RightParen);
            break;
        case '{':
            add_token(token_type::LeftBrace);
            break;
        case '}':
            add_token(token_type::RightBrace);
            break;
        case ',':
            add_token(token_type::Comma);
            break;
        case '.':
            add_token(token_type::Dot);
            break;
        case '?':
            add_token(token_type::QuestionMark);
            break;
        case ';':
            add_token(token_type::Semicolon);
            break;
        case '*':
            add_token(token_type::Star);
            break;
        case '/':
            if (match_next('/')) {
                skip_line();
            } else if (match_next('*')) {
                skip_multiline_comment();
            } else {
                add_token(token_type::Slash);
            }
            break;
        case '!':
            add_token(match_next('=') ? token_type::NotEqual : token_type::Not);
            break;
        case '=':
            add_token(match_next('=') ? token_type::EqualEqual : token_type::Equal);
            break;
        case '<':
            add_token(match_next('=') ? token_type::LessEqual : token_type::Less);
            break;
        case '>':
            add_token(match_next('=') ? token_type::GreaterEqual : token_type::Greater);
            break;
        case '+':
            add_token(match_next('+') ? token_type::PlusPlus : token_type::Plus);
            break;
        case '-':
            add_token(match_next('-') ? token_type::MinusMinus : token_type::Minus);
            break;
        case '&':
            add_token(match_next('&') ? token_type::LogicalAnd : token_type::Ampersand);
            break;
        case '|':
            add_token(match_next('|') ? token_type::LogicalOr : token_type::Pipe);
            break;
        case '~':
            add_token(token_type::Tilde);
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

    bool lexer::is_end() const {
        return current_position >= source.size();
    }

    bool lexer::is_alpha(const char c) {
        return std::isalpha(c) || c == '_';
    }

    char lexer::peek() const {
        return source[current_position];
    }

    char lexer::peek_next() const {
        if (is_end()) {
            return '\0';
        }
        return source[current_position + 1];
    }

    bool lexer::match_next(const char c) {
        const bool match = peek() == c;
        if (match) {
            advance();
        }
        return match;
    }

    void lexer::advance() {
        current_position++;
    }

    std::string lexer::get_lexeme() const {
        return source.substr(start_position, current_position - start_position);
    }

    void lexer::add_token(token_type type, std::optional<int> literal) {
        tokens.emplace_back(type, get_lexeme(), literal);
    }


    //todo handle stirng literals
    void lexer::consume_string() {
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

        add_token(token_type::StringLiteral);
    }

    //todo handle double/float values
    void lexer::consume_digit() {
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
        } else {
            int value = std::stoi(string_value);
            add_token(token_type::IntLiteral, value);
        }
    }

    void lexer::consume_identifier() {
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

    void lexer::skip_line() {
        while (peek() != '\n' && !is_end()) {
            advance();
        }
    }

    void lexer::skip_multiline_comment() {
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

    token_type lexer::keyword_or_identifier(const std::string& str) {
        static const std::unordered_map<std::string_view, token_type> keywords = {
            {"break", token_type::Break},
            {"continue", token_type::Continue},
            {"do", token_type::Do},
            {"else", token_type::Else},
            {"false", token_type::False},
            {"for", token_type::For},
            {"if", token_type::If},
            {"return", token_type::Return},
            {"true", token_type::True},
            {"while", token_type::While},
            {"continue", token_type::Continue},
            {"else", token_type::Else},
            {"false", token_type::False},
            {"for", token_type::For},
            {"if", token_type::If},
            {"return", token_type::Return},
            {"true", token_type::True},
            {"while", token_type::While},
            {"int", token_type::Int},
            {"void", token_type::Void},
            {"float", token_type::Float},
            {"char", token_type::Char},
            {"bool", token_type::Bool},
            {"goto", token_type::Goto},
            {"struct", token_type::Struct},
        };

        if (keywords.contains(str)) {
            return keywords.at(str);
        }

        return token_type::Identifier;
    }
}
