#ifndef Fern_Errors_SourceLocation_hpp
#define Fern_Errors_SourceLocation_hpp

#include <string>

namespace fern {

class SourceLocation {
  int line_;
  int column_;
public:
  SourceLocation() : line_(0), column_(0) {}
  SourceLocation(const int line, const int column) : line_(line), column_(column) {}

  auto operator==(const SourceLocation &other) const -> bool {
    return line_ == other.line_ && column_ == other.column_;
  }

  auto operator!=(const SourceLocation &other) const -> bool {
    return !(*this == other);
  }

  auto advanceColumn() -> void { column_++; }
  auto advanceLine() -> void { line_++; column_ = 0; }

  auto getLine() const -> int { return line_; }
  auto getColumn() const -> int { return column_; }

  // add 1 to line and column to make it 1-indexed
  auto toString() const -> std::string {
    return std::to_string(line_ + 1) + ":" + std::to_string(column_ + 1);
  }
};

}

#endif
