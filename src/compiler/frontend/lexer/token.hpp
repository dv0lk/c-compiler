#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace compiler {
    enum class TokenType {
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        Colon,
        Comma,
        Dot,
        QuestionMark,
        Semicolon,
        Slash,
        Star,
        Not,
        NotEqual,
        Equal,
        EqualEqual,
        Greater,
        GreaterEqual,
        Less,
        LessEqual,
        Minus,
        MinusMinus,
        Plus,
        PlusPlus,
        Percent,
        Ampersand,
        Pipe,
        Caret,
        Tilde,

        PlusEqual,
        MinusEqual,
        StarEqual,
        SlashEqual,
        PercentEqual,
        AmpersandEqual,
        PipeEqual,
        CaretEqual,

        LogicalAnd,
        LogicalOr,

        Identifier,
        StringLiteral,
        IntLiteral,
        DoubleLiteral,

        Int,
        Float,
        Char,
        Bool,
        Void,
        Double,
        If,
        Else,
        While,
        Do,
        For,
        Return,
        Break,
        Continue,
        True,
        False,
        Goto,
        Struct,
    };

    class Token {
    private:
        TokenType type;
        std::string_view lexeme;
        // todo right now we only support int
        std::optional<int> literal;

    public:
        Token(const TokenType type, std::string_view lexeme, const std::optional<int>& literal): type(type), lexeme(lexeme), literal(literal) { }

        Token(const TokenType type, std::string_view lexeme): type(type), lexeme(lexeme) { }

        [[nodiscard]] TokenType get_type() const {
            return type;
        }

        [[nodiscard]] std::optional<int> get_literal() const {
            return literal;
        }

        [[nodiscard]] std::string_view get_lexeme() const {
            return lexeme;
        }

        [[nodiscard]] std::string type_to_string() const noexcept {
            // clang-format off
            static std::unordered_map<TokenType, std::string> map{
                {TokenType::LeftParen, "LeftParen"},
                {TokenType::RightParen, "RightParen"},
                {TokenType::LeftBrace, "LeftBrace"},
                {TokenType::RightBrace, "RightBrace"},
                {TokenType::Colon, "Colon"},
                {TokenType::Comma, "Comma"},
                {TokenType::Dot, "Dot"},
                {TokenType::QuestionMark, "QuestionMark"},
                {TokenType::Semicolon, "Semicolon"},
                {TokenType::Slash, "/"},
                {TokenType::Star, "*"},

                {TokenType::Not, "Not"},
                {TokenType::NotEqual, "NotEqual"},
                {TokenType::Equal, "Equal"},
                {TokenType::EqualEqual, "EqualEqual"},
                {TokenType::Greater, "Greater"},
                {TokenType::GreaterEqual, "GreaterEqual"},
                {TokenType::Less, "Less"},
                {TokenType::LessEqual, "LessEqual"},
                {TokenType::Minus, "-"},
                {TokenType::MinusMinus, "MinusMinus"},
                {TokenType::Plus, "+"},
                {TokenType::PlusPlus, "PlusPlus"},

                {TokenType::Percent, "%"},
                {TokenType::Ampersand, "Ampersand"},
                {TokenType::Pipe, "Pipe"},
                {TokenType::Caret, "Caret"},
                {TokenType::Tilde, "Tilde"},

                {TokenType::PlusEqual, "PlusEqual"},
                {TokenType::MinusEqual, "MinusEqual"},
                {TokenType::StarEqual, "StarEqual"},
                {TokenType::SlashEqual, "SlashEqual"},
                {TokenType::PercentEqual, "PercentEqual"},
                {TokenType::AmpersandEqual, "AmpersandEqual"},
                {TokenType::PipeEqual, "PipeEqual"},
                {TokenType::CaretEqual, "CaretEqual"},

                {TokenType::LogicalAnd, "LogicalAnd"},
                {TokenType::LogicalOr, "LogicalOr"},

                {TokenType::Identifier, "Identifier"},
                {TokenType::StringLiteral, "StringLiteral"},
                {TokenType::IntLiteral, "NumberLiteral"},

                {TokenType::Int, "Int"},
                {TokenType::Float, "Float"},
                {TokenType::Char, "Char"},
                {TokenType::Void, "Void"},
                {TokenType::If, "If"},
                {TokenType::Else, "Else"},
                {TokenType::While, "While"},
                {TokenType::For, "For"},
                {TokenType::Return, "Return"},
                {TokenType::Break, "Break"},
                {TokenType::Continue, "Continue"},
                {TokenType::True, "True"},
                {TokenType::False, "False"},
                {TokenType::Goto, "Goto"},
                {TokenType::Struct, "Struct"}};
            // clang-format on

            const auto it = map.find(this->type);
            if (it != map.end())
                return it->second;

            return "Unknown token type";
        }
    };
} // namespace compiler