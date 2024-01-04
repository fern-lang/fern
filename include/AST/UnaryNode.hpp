#ifndef Fern_Ast_UnaryNode_hpp
#define Fern_Ast_UnaryNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class UnaryNode : public AstNode {
  Token op;
  std::shared_ptr<AstNode> operand;

public:
  UnaryNode(SourceLocation loc, Token op, std::shared_ptr<AstNode> operand) :
      AstNode(loc),
      op(op), operand(operand) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "UnaryNode: '" << op.toString()
        << "'\n";
    operand->print(out, indent + 1);
  }

  auto getOp() const -> Token { return op; }
  auto getOperand() const -> std::shared_ptr<AstNode> { return operand; }
};

} // namespace fern

#endif
