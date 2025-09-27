#pragma once
#include <string>
#include <vector>

#include "token.h"

namespace compiler::lexer {
    class lexer {
    public:
        [[nodiscard]] std::vector<token> parse_tokens(const std::string& input);

        void print_tokens() const;

    private:
        std::string source;
        std::vector<token> tokens;
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

        void add_token(token_type type, std::optional<int> literal = {});

        void consume_string();

        void consume_digit();

        void consume_identifier();

        void skip_line();

        void skip_multiline_comment();

        [[nodiscard]] std::string get_lexeme() const;

        [[nodiscard]] static token_type keyword_or_identifier(const std::string& str);
    };
}

