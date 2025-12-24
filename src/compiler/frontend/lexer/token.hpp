#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

//TODO refactor this
enum class token_t {
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
    token_t type;
    std::string lexeme;
    //todo right now we only support int
    std::optional<int> literal;

public:
    Token(const token_t type, std::string lexeme, const std::optional<int>& literal)
        : type(type),
          lexeme(std::move(lexeme)),
          literal(literal) {}

    Token(const token_t type, std::string lexeme)
        : type(type),
          lexeme(std::move(lexeme)) {}

    [[nodiscard]] token_t get_type() const {
        return type;
    }

    [[nodiscard]] std::optional<int> get_literal() const {
        return literal;
    }

    [[nodiscard]] std::string get_lexeme() const {
        return lexeme;
    }

    [[nodiscard]] std::string type_to_string() const noexcept {
        static std::unordered_map<token_t, std::string> map{
            // Basic punctuation
            {token_t::LeftParen, "LeftParen"},
            {token_t::RightParen, "RightParen"},
            {token_t::LeftBrace, "LeftBrace"},
            {token_t::RightBrace, "RightBrace"},
            {token_t::Colon, "Colon"},
            {token_t::Comma, "Comma"},
            {token_t::Dot, "Dot"},
            {token_t::QuestionMark, "QuestionMark"},
            {token_t::Semicolon, "Semicolon"},
            {token_t::Slash, "/"},
            {token_t::Star, "*"},

            // One or two character tokens
            {token_t::Not, "Not"},
            {token_t::NotEqual, "NotEqual"},
            {token_t::Equal, "Equal"},
            {token_t::EqualEqual, "EqualEqual"},
            {token_t::Greater, "Greater"},
            {token_t::GreaterEqual, "GreaterEqual"},
            {token_t::Less, "Less"},
            {token_t::LessEqual, "LessEqual"},
            {token_t::Minus, "-"},
            {token_t::MinusMinus, "MinusMinus"},
            {token_t::Plus, "+"},
            {token_t::PlusPlus, "PlusPlus"},

            // Additional operators
            {token_t::Percent, "%"},
            {token_t::Ampersand, "Ampersand"},
            {token_t::Pipe, "Pipe"},
            {token_t::Caret, "Caret"},
            {token_t::Tilde, "Tilde"},

            // Compound assignment operators
            {token_t::PlusEqual, "PlusEqual"},
            {token_t::MinusEqual, "MinusEqual"},
            {token_t::StarEqual, "StarEqual"},
            {token_t::SlashEqual, "SlashEqual"},
            {token_t::PercentEqual, "PercentEqual"},
            {token_t::AmpersandEqual, "AmpersandEqual"},
            {token_t::PipeEqual, "PipeEqual"},
            {token_t::CaretEqual, "CaretEqual"},

            // Logical operators
            {token_t::LogicalAnd, "LogicalAnd"},
            {token_t::LogicalOr, "LogicalOr"},

            // Literals
            {token_t::Identifier, "Identifier"},
            {token_t::StringLiteral, "StringLiteral"},
            {token_t::IntLiteral, "NumberLiteral"},

            // Keywords
            {token_t::Int, "Int"},
            {token_t::Float, "Float"},
            {token_t::Char, "Char"},
            {token_t::Void, "Void"},
            {token_t::If, "If"},
            {token_t::Else, "Else"},
            {token_t::While, "While"},
            {token_t::For, "For"},
            {token_t::Return, "Return"},
            {token_t::Break, "Break"},
            {token_t::Continue, "Continue"},
            {token_t::True, "True"},
            {token_t::False, "False"},
            {token_t::Goto, "Goto"},
            {token_t::Struct, "Struct"}
        };

        const auto it = map.find(this->type);
        if (it != map.end())
            return it->second;

        return "Unknown token type";
    }

};
