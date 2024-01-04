#ifndef Fern_Ast_NumberNode_hpp
#define Fern_Ast_NumberNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class NumberNode : public AstNode {
  std::string value;
  bool isFloat;

public:
  NumberNode(SourceLocation loc, std::string value, bool isFloat) :
      AstNode(loc),
      value(value), isFloat(isFloat) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "NumberNode: '" << value << "'\n";
  }

  auto getValue() const -> std::string { return value; }
  auto isFloatValue() const -> bool { return isFloat; }
};

} // namespace fern

#endif
