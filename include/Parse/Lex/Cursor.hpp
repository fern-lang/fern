#ifndef Fern_Parse_Lex_Cursor_hpp
#define Fern_Parse_Lex_Cursor_hpp

#include <Roots/_defines.hpp>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include "../../Errors/Context.hpp"

namespace fern {

class Cursor {
  usize pos;
  std::vector<char> chars;
  SourceLocation location;

public:
  Cursor(Context &context) : pos(0) {
    auto source = context.getSource();
    chars.reserve(source.size());
    for (auto c: source) {
      chars.push_back(c);
    }
  }

  Cursor(const std::string_view source) : pos(0) {
    chars.reserve(source.size());
    for (auto c: source) {
      chars.push_back(c);
    }
  }

  auto isEof() const -> bool { return pos >= chars.size(); }

  auto peek() const -> std::optional<char> {
    if (pos >= chars.size()) {
      return std::nullopt;
    }

    return chars[pos];
  }

  auto peek(usize n) const -> std::optional<char> {
    if (pos + n >= chars.size()) {
      return std::nullopt;
    }

    return chars[pos + n];
  }

  auto next() -> std::optional<char> {
    if (pos >= chars.size()) {
      return std::nullopt;
    }

    auto c = chars[pos++];
    if (c == '\n') {
      location.advanceLine();
    } else {
      location.advanceColumn();
    }

    return c;
  }

  auto nextWhile(std::function<bool(char)> predicate) -> std::string {
    std::string result;
    while (!isEof() && predicate(*peek())) {
      result += *next();
    }

    return result;
  }

  auto getLocation() const -> SourceLocation { return location; }
};

} // namespace fern

#endif
