#include "Parse/Lex/Lexer.hpp"
#include <iostream>

namespace fern {

auto Lexer::isIdStart(char c) -> bool {
  return std::isalpha(c) || c == '_';
}

auto Lexer::isIdContinue(char c) -> bool {
  return std::isalnum(c) || c == '_';
}

auto Lexer::isDigit(char c) -> bool {
  return std::isdigit(c);
}

auto Lexer::lex() -> bool {
  while (!cursor.isEof()) {
    auto tChar = cursor.peek();

    if (!tChar) {
      return false;
    }

    while (std::isspace(*tChar)) {
      cursor.next();
      if (cursor.isEof()) {
        return true;
      }

      tChar = cursor.peek();
    }

    if (isIdStart(*tChar)) {
      auto t = lexIdentifier();
      if (!t) {
        return false;
      }

      tokens.push_back(*t);
    } else if (isDigit(*tChar)) {
      auto t = lexNumber();
      if (!t) {
        return false;
      }

      tokens.push_back(*t);
    } else if (tChar == '"') {
      auto t = lexString();
      if (!t) {
        return false;
      }

      tokens.push_back(*t);
    } else if (tChar == '\'') {
      auto t = lexChar();
      if (!t) {
        return false;
      }

      tokens.push_back(*t);
    } else {
      auto t = lexOperatorOrComment();
      if (!t) {
        return false;
      }

      if (t->getKind() == TokenKind::Comment) {
        continue;
      }

      tokens.push_back(*t);
    }
  }

  return true;
}

auto Lexer::lexIdentifier() -> std::optional<Token> {
  auto start = cursor.getLocation();
  auto lexeme = cursor.nextWhile([this](char c) -> bool {
    return isIdContinue(c);
  });

  auto kind = keywordMap.find(lexeme);
  if (kind != keywordMap.end()) {
    return Token(kind->second, lexeme, start);
  }

  return Token(TokenKind::Ident, lexeme, start);
}

auto Lexer::lexNumber() -> std::optional<Token> {
  auto start = cursor.getLocation();
  auto lexeme = cursor.nextWhile([this](char c) -> bool {
    return isDigit(c);
  });

  if (cursor.peek() == '.') {
    cursor.next();
    lexeme += '.';
    lexeme += cursor.nextWhile([this](char c) -> bool {
      return isDigit(c);
    });

    return Token(TokenKind::Float, lexeme, start);
  }

  return Token(TokenKind::Integer, lexeme, start);
}

auto Lexer::lexString() -> std::optional<Token> {
  auto start = cursor.getLocation();
  bool errorInString = false;
  cursor.next();

  auto lexeme = cursor.nextWhile([&](char c) -> bool {
    if (c == '\n') {
      context.recordError("Newline in string", start);
      errorInString = true;
      return false;
    }

    return c != '"';
  });

  if (cursor.peek() != '"' || cursor.isEof()) {
    context.recordError("Unterminated string", start);
    return std::nullopt;
  }

  cursor.next();

  return Token(TokenKind::String, lexeme, start);
}

auto Lexer::lexChar() -> std::optional<Token> {
  auto start = cursor.getLocation();
  cursor.next();

  if (cursor.peek() == '\'') {
    context.recordError("Empty character literal", start);
    return std::nullopt;
  }

  auto value = *cursor.next();
  if (value == '\n') {
    context.recordError("Newline in character literal", start);
    return std::nullopt;
  }

  if (cursor.peek() != '\'') {
    context.recordError("Unterminated character literal", start);
    return std::nullopt;
  }

  cursor.next();

  return Token(TokenKind::Char, std::string(1, value), start);
}

auto Lexer::lexOperatorOrComment() -> std::optional<Token> {
  auto start = cursor.getLocation();
  auto tChar = cursor.next();

  switch (*tChar) {
    case '+':
      return Token(TokenKind::Plus, "+", start);
    case '-':
      if (cursor.peek() == '>') {
        cursor.next();
        return Token(TokenKind::Arrow, "->", start);
      }
      return Token(TokenKind::Minus, "-", start);
    case '*':
      return Token(TokenKind::Star, "*", start);
    case '/':
      if (cursor.peek() == '/') {
        cursor.next();
        cursor.nextWhile([this](char c) -> bool {
          return c != '\n';
        });

        if (cursor.isEof()) {
          context.recordError("Unterminated line comment", start);
          return std::nullopt;
        }

        return Token(TokenKind::Comment, "", start);
      } else if (cursor.peek() == '*') {
        cursor.next();
        while (!cursor.isEof()) {
          if (cursor.peek() == '*' && cursor.peek(1) == '/') {
            cursor.next();
            cursor.next();
            break;
          }
          cursor.next();
        }

        if (cursor.isEof()) {
          context.recordError("Unterminated block comment", start);
          return std::nullopt;
        }

        return Token(TokenKind::Comment, "", start);
      }
      return Token(TokenKind::Slash, "/", start);
    case '=':
      if (cursor.peek() == '=') {
        cursor.next();
        return Token(TokenKind::EqualEqual, "==", start);
      }
      return Token(TokenKind::Equal, "=", start);
    case '!':
      if (cursor.peek() == '=') {
        cursor.next();
        return Token(TokenKind::BangEqual, "!=", start);
      }
      return Token(TokenKind::Bang, "!", start);
    case '<':
      if (cursor.peek() == '=') {
        cursor.next();
        return Token(TokenKind::LessEqual, "<=", start);
      }
      return Token(TokenKind::Less, "<", start);
    case '>':
      if (cursor.peek() == '=') {
        cursor.next();
        return Token(TokenKind::GreaterEqual, ">=", start);
      }
      return Token(TokenKind::Greater, ">", start);
    case '.':
      return Token(TokenKind::Dot, ".", start);
    case ':':
      if (cursor.peek() == '=') {
        cursor.next();
        return Token(TokenKind::ColonEqual, ":=", start);
      }
      return Token(TokenKind::Colon, ":", start);
    case ';':
      return Token(TokenKind::Semicolon, ";", start);
    case ',':
      return Token(TokenKind::Comma, ",", start);
    case '(':
      return Token(TokenKind::LParen, "(", start);
    case ')':
      return Token(TokenKind::RParen, ")", start);
    case '{':
      return Token(TokenKind::LBrace, "{", start);
    case '}':
      return Token(TokenKind::RBrace, "}", start);
    case '[':
      return Token(TokenKind::LBracket, "[", start);
    case ']':
      return Token(TokenKind::RBracket, "]", start);
    case '&':
      return Token(TokenKind::Ref, "&", start);
    default:
      context.recordError("Unexpected character", start);
      return std::nullopt;
  }
}

}
