#ifndef Fern_Ast_LetNode_hpp
#define Fern_Ast_LetNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "../Sema/Type.hpp"
#include "../Sema/TypeVisitor.hpp"
#include "AstNode.hpp"

namespace fern {

class LetNode : public AstNode {
  std::string name;
  std::optional<Type> typeAnnotation;
  std::shared_ptr<AstNode> value;

public:
  LetNode(SourceLocation loc, std::string name, std::optional<Type> typeAnnotation,
          std::shared_ptr<AstNode> value) :
      AstNode(loc),
      name(name), typeAnnotation(typeAnnotation), value(value) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "LetNode: '" << name << "'";
    if (typeAnnotation) {
      out << " (ty annot: " << typeAnnotation->getTypeName() << ")";
    }
    out << "\n";
    value->print(out, indent + 1);
  }

  auto typeCheck(TypeVisitor &visitor) -> void override { visitor.visit(*this); }
  auto codegen(CodegenVisitor &visitor) -> llvm::Value * override { return visitor.visit(*this); }

  auto getName() const -> std::string { return name; }
  auto getTypeAnnotation() const -> std::optional<Type> { return typeAnnotation; }
  auto getValue() const -> std::shared_ptr<AstNode> { return value; }
};

} // namespace fern

#endif
