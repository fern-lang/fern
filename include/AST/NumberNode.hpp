#ifndef Fern_Ast_NumberNode_hpp
#define Fern_Ast_NumberNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class NumberNode : public AstNode {
  std::string value;
  bool isFloat;

public:
  NumberNode(SourceLocation loc, std::string value, bool isFloat) :
      AstNode(loc),
      value(value), isFloat(isFloat)
  {
    setType(isFloat ? Type::Float() : Type::Int());    
  }

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "NumberNode: '" << value << "'\n";
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value* override  { return visitor.visit(*this); }

  auto getValue() const -> std::string { return value; }
  auto isFloatValue() const -> bool { return isFloat; }
};

} // namespace fern

#endif
