#ifndef Fern_Ast_CallNode_hpp
#define Fern_Ast_CallNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class CallNode : public AstNode {
  std::string callee;
  std::vector<std::shared_ptr<AstNode>> args;

public:
  CallNode(SourceLocation loc, std::string callee,
           std::vector<std::shared_ptr<AstNode>> args) :
      AstNode(loc),
      callee(callee), args(args) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "CallNode: '" << callee << "'\n";
    for (auto &arg: args) {
      arg->print(out, indent + 1);
    }
  }

  auto getCallee() const -> std::string { return callee; }
  auto getArgs() const -> std::vector<std::shared_ptr<AstNode>> { return args; }
};

} // namespace fern

#endif
