#ifndef Fern_Ast_VariableNode_hpp
#define Fern_Ast_VariableNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class VariableNode : public AstNode {
  std::string name;

public:
  VariableNode(SourceLocation loc, std::string name) :
      AstNode(loc),
      name(name) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "VariableNode: '" << name << "'\n";
  }

  auto getName() const -> std::string { return name; }
};

} // namespace fern

#endif
