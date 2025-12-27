#pragma once
#include <string>
#include <vector>

#include "token.hpp"

namespace compiler::lexer {
    class Tokenizer {
    public:
        [[nodiscard]] std::vector<Token> parse_tokens(const std::string& source);

        [[nodiscard]] static std::vector<Token> get_tokens(const std::string& source) {
            Tokenizer t;
            return t.parse_tokens(source);
        }

        void print_tokens() const;

    private:
        std::string source;
        std::vector<Token> tokens;
        int start_position = 0;
        int current_position = 0;
        int line = 1;

        void lex();

        [[nodiscard]] bool is_end() const;

        [[nodiscard]] bool is_alpha(char c);

        [[nodiscard]] char peek() const;

        [[nodiscard]] char peek_next() const;

        [[nodiscard]] bool match_next(char c);

        void advance();

        void add_token(TokenType type, std::optional<int> literal = {});

        void consume_string();

        void consume_digit();

        void consume_identifier();

        void skip_line();

        void skip_multiline_comment();

        [[nodiscard]] std::string get_lexeme() const;

        [[nodiscard]] static TokenType keyword_or_identifier(const std::string& str);
    };
} // namespace compiler::lexer
