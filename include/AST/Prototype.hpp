#ifndef Fern_Ast_Prototype_hpp
#define Fern_Ast_Prototype_hpp

#include <Roots/_defines.hpp>
#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "llvm/Support/raw_ostream.h"

namespace fern {

struct PrototypeArg {
  std::string name;
  Token type;

  PrototypeArg(std::string name, Token type) : name(name), type(type) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out << std::string(indent * 2, ' ') << "PrototypeArg: '" << name
        << "' (ty: " << type.getLexeme() << ")\n";
  }
};

class Prototype {
  std::string name;
  std::vector<std::shared_ptr<PrototypeArg>> args;
  Token returnType;

public:
  Prototype(std::string name, std::vector<std::shared_ptr<PrototypeArg>> args,
            Token returnType) :
      name(name),
      args(args), returnType(returnType) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out << std::string(indent * 2, ' ') << "Prototype: '" << name
        << "' (ret ty: " << returnType.getLexeme() << ")\n";
    for (auto &arg: args) {
      arg->print(out, indent + 1);
    }
  }

  auto getName() const -> std::string { return name; }
  auto getArgs() const -> std::vector<std::shared_ptr<PrototypeArg>> { return args; }
  auto getReturnType() const -> Token { return returnType; }
};

} // namespace fern


#endif
