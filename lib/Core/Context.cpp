#include "Errors/Context.hpp"

namespace fern {

auto Context::recordError(const std::string &message, SourceLocation loc) -> void {
  errors.push_back(Error(message, loc));
}

auto Context::recordWarning(const std::string &message, SourceLocation loc) -> void {
  warnings.push_back(Error(message, loc, true));
}

auto Context::recordNote(const std::string &message) -> void {
  errors.back().addNote(message);
}

} // namespace fern
