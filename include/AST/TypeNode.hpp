#ifndef Fern_Ast_TypeNode_hpp
#define Fern_Ast_TypeNode_hpp

#include <memory>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "AstNode.hpp"

namespace fern {

class TypeNode : public AstNode {
  std::string typeName;
  bool isPointer;
  int pointerDepth;

public:
  TypeNode(SourceLocation loc, std::string typeName, bool isPointer, int pointerDepth) :
      AstNode(loc), typeName(typeName), isPointer(isPointer), pointerDepth(pointerDepth) {}

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out << std::string(indent * 2, ' ') << "TypeNode: '" << typeName << "' (ptr: " << isPointer << ", depth: " << pointerDepth << ")\n";
  }

  auto getTypeName() const -> std::string { return typeName; }
  auto isPointerType() const -> bool { return isPointer; }
  auto getPointerDepth() const -> int { return pointerDepth; }
};

}

#endif
