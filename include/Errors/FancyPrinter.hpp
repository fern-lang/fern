#ifndef Fern_Errors_FancyPrinter_hpp
#define Fern_Errors_FancyPrinter_hpp

#include <Roots/_defines.hpp>
#include <fmt/format.h>
#include <string>
#include <string_view>
#include "Error.hpp"

namespace fern {

class FancyErrorPrinter {
  std::string_view source;
  std::string_view filename;

public:
  FancyErrorPrinter(std::string_view source, std::string_view filename) :
      source(source), filename(filename) {}

  auto print(const Error &error) const -> void {
    /*
     * ./test.fern:3:5: error: expected identifier
     *   |
     * 3 | let 1 = 2
     *   |     ^
     */

    auto loc = error.getLocation();
    auto line = loc.getLine();
    auto col = loc.getColumn();

    std::cerr << fmt::format("{}:{}:{}: {}: {}\n", filename, line + 1, col + 1,
                             error.isWarning() ? "warning" : "error", error.getMessage());
    std::cerr << fmt::format("{} |\n",
                             std::string(std::to_string(line + 1).length(), ' '));
    std::cerr << fmt::format("{} | {}", line + 1, getSourceLine(line)) << std::endl;
    std::cerr << fmt::format("{} | {}\n",
                             std::string(std::to_string(line + 1).length(), ' '), std::string(col, ' ') + "^");
  }

private:
  auto getSourceLine(usize line) const -> std::string {
    usize start = 0;
    usize end = source.find('\n', start);

    for (usize i = 0; i < line; i++) {
      start = end + 1;
      end = source.find('\n', start);
    }

    return std::string(source.substr(start, end - start));
  }
};

} // namespace fern

#endif
