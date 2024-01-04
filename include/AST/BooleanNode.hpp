#ifndef Fern_Ast_BooleanNode_hpp
#define Fern_Ast_BooleanNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class BooleanNode : public AstNode {
  bool value;

public:
  BooleanNode(SourceLocation loc, bool value) : AstNode(loc), value(value) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "BooleanNode: '" << value << "'\n";
  }

  auto getValue() const -> bool { return value; }
};

} // namespace fern

#endif
