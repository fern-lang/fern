#ifndef Fern_Ast_LetNode_hpp
#define Fern_Ast_LetNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class LetNode : public AstNode {
  std::string name;
  Token type;
  std::shared_ptr<AstNode> value;

public:
  LetNode(SourceLocation loc, std::string name, Token type,
         std::shared_ptr<AstNode> value) :
      AstNode(loc),
      name(name), type(type), value(value) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "LetNode: '" << name << "' (ty: " << type.getLexeme() << ")\n";
    value->print(out, indent + 1);
  }

  auto getName() const -> std::string { return name; }
  auto getType() const -> Token { return type; }
  auto getValue() const -> std::shared_ptr<AstNode> { return value; }
};

} // namespace fern

#endif
