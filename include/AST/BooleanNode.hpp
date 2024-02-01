#ifndef Fern_Ast_BooleanNode_hpp
#define Fern_Ast_BooleanNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class BooleanNode : public AstNode {
  bool value;

public:
  BooleanNode(SourceLocation loc, bool value) : AstNode(loc), value(value) {
    setType(Type::Bool());
  }

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "BooleanNode: '" << value << "'\n";
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value* override  { return visitor.visit(*this); }

  auto getValue() const -> bool { return value; }
};

} // namespace fern

#endif
