#ifndef Fern_Parse_ParseContext_hpp
#define Fern_Parse_ParseContext_hpp

#include <string>
#include <vector>

#include "../AST/ExternDef.hpp"
#include "../AST/Function.hpp"
#include "Error.hpp"
#include "FancyPrinter.hpp"

namespace fern {

class Context {
  std::vector<Error> errors;
  std::vector<Error> warnings;
  std::string_view source;
  std::string_view filename;
  std::vector<std::shared_ptr<Function>> functions;
  std::vector<std::shared_ptr<ExternDef>> externs;

public:
  Context(std::string_view source, std::string_view filename) :
      source(source), filename(filename) {}

  auto recordError(const std::string &message, SourceLocation loc) -> void;
  auto recordWarning(const std::string &message, SourceLocation loc) -> void;

  auto printErrors(FancyErrorPrinter &printer) const -> void {
    if (!hasErrors()) {
      std::cerr << "No errors recorded" << std::endl;
      return;
    }

    for (auto &err: errors) {
      printer.print(err);
    }

    std::cerr << fmt::format("Generated {} error{}", getErrors().size(),
                             getErrors().size() == 1 ? "" : "s")
              << std::endl;
  }

  auto flushWarnings(FancyErrorPrinter &printer) -> void {
    for (auto &warn: warnings) {
      printer.print(warn);
    }
    warnings.clear();
  }

  auto addFunction(std::shared_ptr<Function> func) -> void { functions.push_back(func); }

  auto addExtern(std::shared_ptr<ExternDef> ext) -> void { externs.push_back(ext); }

  auto hasErrors() const -> bool { return !errors.empty(); }

  auto getSource() const -> std::string_view { return source; }
  auto getFilename() const -> std::string_view { return filename; }
  auto getErrors() const -> const std::vector<Error> & { return errors; }
  auto getFunctions() const -> const std::vector<std::shared_ptr<Function>> & {
    return functions;
  }
  auto getExterns() const -> const std::vector<std::shared_ptr<ExternDef>> & {
    return externs;
  }
};

} // namespace fern

#endif
