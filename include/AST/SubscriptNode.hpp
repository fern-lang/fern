#ifndef Fern_Ast_SubscriptNode_hpp
#define Fern_Ast_SubscriptNode_hpp

#include <memory>
#include <string>
#include "AstNode.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class SubscriptNode : public AstNode {
  std::shared_ptr<AstNode> operand;
  std::shared_ptr<AstNode> index;

public:
  SubscriptNode(SourceLocation loc, std::shared_ptr<AstNode> operand, std::shared_ptr<AstNode> index) :
      AstNode(loc),
      operand(operand), index(index) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "SubscriptNode:\n";
    operand->print(out, indent + 1);
    index->print(out, indent + 1);
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value * override  { return visitor.visit(*this); }

  auto getOperand() const -> std::shared_ptr<AstNode> { return operand; }
  auto getIndex() const -> std::shared_ptr<AstNode> { return index; }
  auto getIndexedType() const -> Type { return operand->getType().deref(); }
};

} // namespace fern

#endif
