#ifndef Fern_Ast_ExternDef_hpp
#define Fern_Ast_ExternDef_hpp

#include <Roots/_defines.hpp>
#include <memory>
#include <string>
#include "Prototype.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class ExternDef {
  std::shared_ptr<Prototype> proto;

public:
  ExternDef(std::shared_ptr<Prototype> proto) : proto(proto) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out.indent(indent) << "ExternDef\n";
    proto->print(out, indent + 1);
  }

  auto typeCheck(TypeVisitor &visitor) -> void { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> void { visitor.visit(*this); }

  auto getProto() const -> std::shared_ptr<Prototype> { return proto; }
  auto getName() const -> const std::string { return proto->getName(); }
};

} // namespace fern

#endif
