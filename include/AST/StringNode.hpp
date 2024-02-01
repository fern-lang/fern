#ifndef Fern_Ast_StringNode_hpp
#define Fern_Ast_StringNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"
#include "../Sema/TypeVisitor.hpp"

namespace fern {

class StringNode : public AstNode {
  std::string value;
  bool isChar;

public:
  StringNode(SourceLocation loc, std::string value) :
      AstNode(loc),
      value(value)
  {
    setType(Type::Str());    
  }

  virtual auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "StringNode: \"" << value << "\"\n";
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value* override  { return visitor.visit(*this); }

  auto getValue() const -> std::string { return value; }
};

} // namespace fern

#endif
