#ifndef Fern_Ast_VariableNode_hpp
#define Fern_Ast_VariableNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class VariableNode : public AstNode {
  std::string name;

public:
  VariableNode(SourceLocation loc, std::string name) :
      AstNode(loc),
      name(name) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "VariableNode: '" << name << "'\n";
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value* override  { return visitor.visit(*this); }

  auto getName() const -> std::string { return name; }
};

} // namespace fern

#endif
