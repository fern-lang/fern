#ifndef Fern_Ast_BinaryNode_hpp
#define Fern_Ast_BinaryNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class BinaryNode : public AstNode {
  TokenKind op;
  std::shared_ptr<AstNode> lhs, rhs;

public:
  BinaryNode(SourceLocation loc, TokenKind op, std::shared_ptr<AstNode> lhs,
             std::shared_ptr<AstNode> rhs) :
      AstNode(loc),
      op(op), lhs(lhs), rhs(rhs) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "BinaryNode: '" << tokenKindToString(op)
        << "'\n";
    lhs->print(out, indent + 1);
    rhs->print(out, indent + 1);
  }

  auto getOp() -> TokenKind const { return op; }
  auto getLhs() const -> std::shared_ptr<AstNode> { return lhs; }
  auto getRhs() const -> std::shared_ptr<AstNode> { return rhs; }
};

} // namespace fern

#endif
