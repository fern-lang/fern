#ifndef Fern_Parse_ParseContext_hpp
#define Fern_Parse_ParseContext_hpp

#include <string>
#include <vector>

#include "../AST/ExternDef.hpp"
#include "../AST/Function.hpp"
#include "Error.hpp"
#include "FancyPrinter.hpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace fern {

class Context {
  std::vector<Error> errors;
  std::vector<Error> warnings;
  std::string_view source;
  std::string_view filename;

  llvm::LLVMContext llvmContext;
  llvm::IRBuilder<> builder{llvmContext};
  llvm::Module llvmModule{"main", llvmContext};

public:
  Context(std::string_view source, std::string_view filename) :
      source(source), filename(filename) {}

  auto recordError(const std::string &message, SourceLocation loc) -> void;
  auto recordWarning(const std::string &message, SourceLocation loc) -> void;

  // sets the note of the previous error
  auto recordNote(const std::string &message) -> void;

  auto printErrors(FancyErrorPrinter &printer) -> void {
    flushWarnings(printer);
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

  auto hasErrors() const -> bool { return !errors.empty(); }

  auto getSource() const -> std::string_view { return source; }
  auto getFilename() const -> std::string_view { return filename; }
  auto getErrors() const -> const std::vector<Error> & { return errors; }

  auto getLLVMContext() -> llvm::LLVMContext & { return llvmContext; }
  auto getBuilder() -> llvm::IRBuilder<> & { return builder; }
  auto getModule() -> llvm::Module & { return llvmModule; }
};

} // namespace fern

#endif
