#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>


enum class token_type {
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

class token {
private:
    token_type type;
    std::string lexeme;
    //todo right now we only support int
    std::optional<int> literal;

public:
    token(const token_type type, std::string lexeme, const std::optional<int>& literal)
        : type(type),
          lexeme(std::move(lexeme)),
          literal(literal) {}

    token(const token_type type, std::string lexeme)
        : type(type),
          lexeme(std::move(lexeme)) {}

    [[nodiscard]] token_type get_type() const {
        return type;
    }

    [[nodiscard]] std::optional<int> get_literal() const {
        return literal;
    }

    [[nodiscard]] std::string get_lexeme() const {
        return lexeme;
    }

    [[nodiscard]] std::string type_to_string() const noexcept {
        static std::unordered_map<token_type, std::string> map{
            // Basic punctuation
            {token_type::LeftParen, "LeftParen"},
            {token_type::RightParen, "RightParen"},
            {token_type::LeftBrace, "LeftBrace"},
            {token_type::RightBrace, "RightBrace"},
            {token_type::Colon, "Colon"},
            {token_type::Comma, "Comma"},
            {token_type::Dot, "Dot"},
            {token_type::QuestionMark, "QuestionMark"},
            {token_type::Semicolon, "Semicolon"},
            {token_type::Slash, "/"},
            {token_type::Star, "*"},

            // One or two character tokens
            {token_type::Not, "Not"},
            {token_type::NotEqual, "NotEqual"},
            {token_type::Equal, "Equal"},
            {token_type::EqualEqual, "EqualEqual"},
            {token_type::Greater, "Greater"},
            {token_type::GreaterEqual, "GreaterEqual"},
            {token_type::Less, "Less"},
            {token_type::LessEqual, "LessEqual"},
            {token_type::Minus, "-"},
            {token_type::MinusMinus, "MinusMinus"},
            {token_type::Plus, "+"},
            {token_type::PlusPlus, "PlusPlus"},

            // Additional operators
            {token_type::Percent, "%"},
            {token_type::Ampersand, "Ampersand"},
            {token_type::Pipe, "Pipe"},
            {token_type::Caret, "Caret"},
            {token_type::Tilde, "Tilde"},

            // Compound assignment operators
            {token_type::PlusEqual, "PlusEqual"},
            {token_type::MinusEqual, "MinusEqual"},
            {token_type::StarEqual, "StarEqual"},
            {token_type::SlashEqual, "SlashEqual"},
            {token_type::PercentEqual, "PercentEqual"},
            {token_type::AmpersandEqual, "AmpersandEqual"},
            {token_type::PipeEqual, "PipeEqual"},
            {token_type::CaretEqual, "CaretEqual"},

            // Logical operators
            {token_type::LogicalAnd, "LogicalAnd"},
            {token_type::LogicalOr, "LogicalOr"},

            // Literals
            {token_type::Identifier, "Identifier"},
            {token_type::StringLiteral, "StringLiteral"},
            {token_type::IntLiteral, "NumberLiteral"},

            // Keywords
            {token_type::Int, "Int"},
            {token_type::Float, "Float"},
            {token_type::Char, "Char"},
            {token_type::Void, "Void"},
            {token_type::If, "If"},
            {token_type::Else, "Else"},
            {token_type::While, "While"},
            {token_type::For, "For"},
            {token_type::Return, "Return"},
            {token_type::Break, "Break"},
            {token_type::Continue, "Continue"},
            {token_type::True, "True"},
            {token_type::False, "False"},
            {token_type::Goto, "Goto"},
            {token_type::Struct, "Struct"}
        };

        const auto it = map.find(this->type);
        if (it != map.end())
            return it->second;

        return "Unknown token type";
    }

};
