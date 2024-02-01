#ifndef Fern_Parse_Lex_Token_hpp
#define Fern_Parse_Lex_Token_hpp

#include <string>
#include "../SourceLocation.hpp"

namespace fern {

enum class TokenKind {
  // Punctuation
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Comma,
  Dot,
  Colon,
  Semicolon,
  Ref,
  
  // Operators
  Plus,
  Minus,
  Star,
  Slash,
  Arrow, // ->
  Equal,
  EqualEqual,
  ColonEqual,
  Bang,
  BangEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,

  // Literals
  Ident,
  String,
  Char,
  Integer,
  Float,
  Bool,

  // Keywords
  Let,
  If,
  Else,
  For,
  While,
  Return,
  Break,
  Continue,
  True,
  False,
  Nil,
  Func,
  Extern,

  // Special
  Eof,
  Invalid,
  Comment,
};

auto tokenKindToString(TokenKind kind) -> std::string;

class Token {
  TokenKind kind;
  std::string lexeme;
  SourceLocation location;

public:
  Token(TokenKind kind, std::string lexeme, SourceLocation location)
      : kind(kind), lexeme(lexeme), location(location) {}

  auto operator==(const TokenKind &other) const -> bool {
    return kind == other;
  }

  auto getKind() const -> TokenKind { return kind; }
  auto getLexeme() const -> std::string { return lexeme; }
  auto getLocation() const -> SourceLocation { return location; }

  auto toString() const -> std::string {
    return tokenKindToString(kind) + " - '" + lexeme + "' at " +
           location.toString();
  }

  static auto makeInvalid() -> Token {
    return Token(TokenKind::Invalid, "", SourceLocation());
  }
};

}

#endif
