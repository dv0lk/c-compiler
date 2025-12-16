#include "tokenizer.h"

#include <iostream>
#include <stdexcept>

namespace compiler::lexer {
    std::vector<token> tokenizer::parse_tokens(const std::string& source) {
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
            add_token(token_t::LeftParen);
            break;
        case ')':
            add_token(token_t::RightParen);
            break;
        case '{':
            add_token(token_t::LeftBrace);
            break;
        case '}':
            add_token(token_t::RightBrace);
            break;
        case ',':
            add_token(token_t::Comma);
            break;
        case '.':
            add_token(token_t::Dot);
            break;
        case '?':
            add_token(token_t::QuestionMark);
            break;
        case ';':
            add_token(token_t::Semicolon);
            break;
        case '*':
            add_token(token_t::Star);
            break;
        case '/':
            if (match_next('/')) {
                skip_line();
            } else if (match_next('*')) {
                skip_multiline_comment();
            } else {
                add_token(token_t::Slash);
            }
            break;
        case '!':
            add_token(match_next('=') ? token_t::NotEqual : token_t::Not);
            break;
        case '=':
            add_token(match_next('=') ? token_t::EqualEqual : token_t::Equal);
            break;
        case '<':
            add_token(match_next('=') ? token_t::LessEqual : token_t::Less);
            break;
        case '>':
            add_token(match_next('=') ? token_t::GreaterEqual : token_t::Greater);
            break;
        case '+':
            add_token(match_next('+') ? token_t::PlusPlus : token_t::Plus);
            break;
        case '-':
            add_token(match_next('-') ? token_t::MinusMinus : token_t::Minus);
            break;
        case '&':
            add_token(match_next('&') ? token_t::LogicalAnd : token_t::Ampersand);
            break;
        case '|':
            add_token(match_next('|') ? token_t::LogicalOr : token_t::Pipe);
            break;
        case '~':
            add_token(token_t::Tilde);
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

    void tokenizer::add_token(token_t type, std::optional<int> literal) {
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

        add_token(token_t::StringLiteral);
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
        add_token(token_t::IntLiteral, value);
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

    token_t tokenizer::keyword_or_identifier(const std::string& str) {
        static const std::unordered_map<std::string_view, token_t> keywords = {
            {"break", token_t::Break},
            {"continue", token_t::Continue},
            {"do", token_t::Do},
            {"else", token_t::Else},
            {"false", token_t::False},
            {"for", token_t::For},
            {"if", token_t::If},
            {"return", token_t::Return},
            {"true", token_t::True},
            {"while", token_t::While},
            {"continue", token_t::Continue},
            {"else", token_t::Else},
            {"false", token_t::False},
            {"for", token_t::For},
            {"if", token_t::If},
            {"return", token_t::Return},
            {"true", token_t::True},
            {"while", token_t::While},
            {"int", token_t::Int},
            {"void", token_t::Void},
            {"float", token_t::Float},
            {"char", token_t::Char},
            {"bool", token_t::Bool},
            {"goto", token_t::Goto},
            {"struct", token_t::Struct},
        };

        if (keywords.contains(str)) {
            return keywords.at(str);
        }

        return token_t::Identifier;
    }
}
