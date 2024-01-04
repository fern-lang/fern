#ifndef Fern_Ast_IfNode_hpp
#define Fern_Ast_IfNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class IfNode : public AstNode {
  std::shared_ptr<AstNode> condition, thenBlock, elseBlock;

public:
  IfNode(SourceLocation loc, std::shared_ptr<AstNode> condition,
         std::shared_ptr<AstNode> thenBlock, std::shared_ptr<AstNode> elseBlock) :
      AstNode(loc),
      condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "IfNode:\n";
    condition->print(out, indent + 1);
    thenBlock->print(out, indent + 1);
    if (elseBlock) {
      elseBlock->print(out, indent + 1);
    }
  }

  auto getCondition() const -> std::shared_ptr<AstNode> { return condition; }
  auto getThenBlock() const -> std::shared_ptr<AstNode> { return thenBlock; }
  auto hasElseBlock() const -> bool { return elseBlock != nullptr; }
  auto getElseBlock() const -> std::shared_ptr<AstNode> { return elseBlock; }
};

} // namespace fern

#endif
