#ifndef Fern_Ast_SingleOpNode_hpp
#define Fern_Ast_SingleOpNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class SingleOpNode : public AstNode {
  TokenKind op; // return, break, continue
  std::shared_ptr<AstNode> expr; // return expr

public:
  SingleOpNode(SourceLocation loc, TokenKind op, std::shared_ptr<AstNode> expr) :
      AstNode(loc), op(op), expr(expr) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "SingleOpNode: '" << tokenKindToString(op)
        << "'\n";
    if (expr) {
      expr->print(out, indent + 1);
    }
  }

  auto getOp() -> TokenKind const { return op; }
  auto getExpr() const -> std::shared_ptr<AstNode> { return expr; }
};

} // namespace fern

#endif
