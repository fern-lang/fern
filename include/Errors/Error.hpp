#ifndef Fern_Errors_Error_hpp
#define Fern_Errors_Error_hpp

#include <string>
#include "../Parse/SourceLocation.hpp"

namespace fern {

class Error {
  std::string message_;
  SourceLocation location_;
  bool isWarning_;

public:
  Error(const std::string &message, const SourceLocation &location, bool isWarning = false)
      : message_(message), location_(location), isWarning_(isWarning) {}
  ~Error() = default;

  auto getMessage() const -> std::string { return message_; }
  auto getLocation() const -> SourceLocation { return location_; }
  auto isWarning() const -> bool { return isWarning_; }

  auto toString() const -> std::string {
    return location_.toString() + (isWarning_ ? " warn" : " err") + ": " + message_;
  }
};

} // namespace fern

#endif
