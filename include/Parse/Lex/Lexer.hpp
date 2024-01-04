#ifndef Fern_Parse_Lex_Lexer_hpp
#define Fern_Parse_Lex_Lexer_hpp

#include <Roots/Error.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "Token.hpp"
#include "Cursor.hpp"
#include "../../Errors/Context.hpp"
#include "../SourceLocation.hpp"

namespace fern {

class Lexer {
  Cursor cursor;
  Context &context;
  std::vector<Token> tokens;

  std::map<std::string, TokenKind> keywordMap = {
    {"let", TokenKind::Let},
    {"if", TokenKind::If},
    {"else", TokenKind::Else},
    {"for", TokenKind::For},
    {"while", TokenKind::While},
    {"return", TokenKind::Return},
    {"break", TokenKind::Break},
    {"continue", TokenKind::Continue},
    {"true", TokenKind::True},
    {"false", TokenKind::False},
    {"nil", TokenKind::Nil},
    {"func", TokenKind::Func},
    {"extern", TokenKind::Extern},
  };

public:
  Lexer(Context &ctx) : context(ctx), cursor(ctx) {}

  auto lex() -> bool;
  auto getTokens() -> std::vector<Token> & { return tokens; }

private:
  
  auto lexIdentifier() -> std::optional<Token>;
  auto lexNumber() -> std::optional<Token>;
  auto lexString() -> std::optional<Token>;
  auto lexChar() -> std::optional<Token>;
  auto lexOperatorOrComment() -> std::optional<Token>;

  auto isIdStart(char c) -> bool;
  auto isIdContinue(char c) -> bool;
  auto isDigit(char c) -> bool;
};

} // namespace fern

#endif
