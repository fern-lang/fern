#ifndef Fern_Ast_Prototype_hpp
#define Fern_Ast_Prototype_hpp

#include <Roots/_defines.hpp>
#include <memory>
#include <string>
#include "../Codegen/CodegenVisitor.hpp"
#include "../Parse/Lex/Token.hpp"
#include "../Parse/SourceLocation.hpp"
#include "../Sema/Type.hpp"
#include "../Sema/TypeVisitor.hpp"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

namespace fern {

struct PrototypeArg {
  std::string name;
  Type type;

  PrototypeArg(std::string name, Type type) : name(name), type(type) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out.indent(indent) << "PrototypeArg: '" << name << "' (ty: " << type.getTypeName()
                       << ")\n";
  }
};

class Prototype {
  std::string name;
  std::vector<std::shared_ptr<PrototypeArg>> args;
  Type returnType;
  SourceLocation loc;

public:
  Prototype(std::string name, std::vector<std::shared_ptr<PrototypeArg>> args,
            Type returnType, SourceLocation loc) :
      name(name),
      args(args), returnType(returnType), loc(loc) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out.indent(indent) << "Prototype: '" << name
                       << "' (ret ty: " << returnType.getTypeName() << ")\n";
    for (auto &arg: args) {
      arg->print(out, indent + 1);
    }
  }

  auto typeCheck(TypeVisitor &visitor) -> void { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Function * {
    return visitor.visit(*this);
  }

  auto getName() const -> std::string { return name; }
  auto getArgs() const -> std::vector<std::shared_ptr<PrototypeArg>> { return args; }
  auto getArgTypes() const -> std::vector<Type> {
    std::vector<Type> types;
    for (auto &arg: args) {
      types.push_back(arg->type);
    }
    return types;
  }
  auto getReturnType() const -> Type { return returnType; }
  auto getLocation() const -> SourceLocation { return loc; }
};

} // namespace fern


#endif
