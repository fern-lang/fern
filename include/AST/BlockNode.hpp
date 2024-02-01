#ifndef Fern_Ast_BlockNode_hpp
#define Fern_Ast_BlockNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "../Sema/TypeVisitor.hpp"
#include "AstNode.hpp"

namespace fern {

class BlockNode : public AstNode {
  std::vector<std::shared_ptr<AstNode>> nodes;

public:
  BlockNode(SourceLocation loc, std::vector<std::shared_ptr<AstNode>> nodes) :
      AstNode(loc), nodes(nodes) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "BlockNode\n";
    for (auto &node: nodes) {
      node->print(out, indent + 1);
    }
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value * override {
    return visitor.visit(*this);
  }

  auto getNodes() const -> std::vector<std::shared_ptr<AstNode>> { return nodes; }
};

} // namespace fern

#endif
