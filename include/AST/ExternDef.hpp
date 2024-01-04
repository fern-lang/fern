#ifndef Fern_Ast_ExternDef_hpp
#define Fern_Ast_ExternDef_hpp

#include <Roots/_defines.hpp>
#include <memory>
#include <string>
#include "Prototype.hpp"

namespace fern {

class ExternDef {
  std::shared_ptr<Prototype> proto;

public:
  ExternDef(std::shared_ptr<Prototype> proto) : proto(proto) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out << std::string(indent * 2, ' ') << "ExternDef\n";
    proto->print(out, indent + 1);
  }

  auto getProto() const -> std::shared_ptr<Prototype> { return proto; }
};

} // namespace fern

#endif
