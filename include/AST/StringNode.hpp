#ifndef Fern_Ast_StringNode_hpp
#define Fern_Ast_StringNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class StringNode : public AstNode {
  std::string value;
  bool isChar;

public:
  StringNode(SourceLocation loc, std::string value) :
      AstNode(loc),
      value(value) {}

  virtual auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "StringNode: \"" << value << "\"\n";
  }

  auto getValue() const -> std::string { return value; }
};

} // namespace fern

#endif
