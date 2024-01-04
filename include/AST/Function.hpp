#ifndef Fern_Ast_Function_hpp
#define Fern_Ast_Function_hpp

#include <memory>
#include <string>
#include "AstNode.hpp"
#include "Prototype.hpp"

namespace fern {

class Function {
  std::shared_ptr<Prototype> proto;
  std::shared_ptr<AstNode> body;

public:
  Function(std::shared_ptr<Prototype> proto, std::shared_ptr<AstNode> body) :
      proto(proto), body(body) {}

  void print(llvm::raw_fd_ostream &out, usize indent) {
    out << std::string(indent * 2, ' ') << "Function\n";
    proto->print(out, indent + 1);
    body->print(out, indent + 1);
  }

  auto getProto() const -> std::shared_ptr<Prototype> { return proto; }
  auto getBody() const -> std::shared_ptr<AstNode> { return body; }
};

} // namespace fern

#endif
