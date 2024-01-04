#ifndef Fern_Parse_TokenIterator_hpp
#define Fern_Parse_TokenIterator_hpp

#include <optional>
#include <vector>
#include <Roots/_defines.hpp>
#include "Lex/Token.hpp"

namespace fern {

class TokenIterator {
  std::vector<Token> tokens;
  usize pos;

public:
  TokenIterator(const std::vector<Token> &tokens) : tokens(std::move(tokens)), pos(0) {}

  auto isEof() const -> bool { return pos >= tokens.size(); }

  auto peek() const -> std::optional<Token> {
    if (pos >= tokens.size()) {
      return std::nullopt;
    }

    return tokens[pos];
  }

  auto peek(usize n) const -> std::optional<Token> {
    if (pos + n >= tokens.size()) {
      return std::nullopt;
    }

    return tokens[pos + n];
  }

  auto next() -> std::optional<Token> {
    if (pos >= tokens.size()) {
      return std::nullopt;
    }

    return tokens[pos++];
  }
};

}

#endif
