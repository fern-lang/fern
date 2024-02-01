#ifndef Fern_Ast_AstNode_hpp
#define Fern_Ast_AstNode_hpp

#include <Roots/_defines.hpp>
#include <iostream>
#include "../Parse/SourceLocation.hpp"
#include "../Sema/Type.hpp"
#include "../Sema/TypeVisitor.hpp"
#include "../Codegen/CodegenVisitor.hpp"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

namespace fern {

class AstNode {
  SourceLocation loc;
  Type type = Type::Invalid();

public:
  AstNode(SourceLocation loc) : loc(loc) {}

  auto getLocation() const -> SourceLocation { return loc; }

  template<typename T>
  auto is() const -> bool {
    return typeid(T) == typeid(*this);
  }
  template<typename T>
  auto as() -> T * {
    return is<T>() ? static_cast<T *>(this) : nullptr;
  }
  template<typename T>
  auto as_ref() -> T & {
    return *as<T>();
  }

  virtual auto print(llvm::raw_fd_ostream &out, usize indent) const -> void = 0;
  virtual auto typeCheck(TypeVisitor &visitor) -> void = 0;
  virtual auto codegen(CodegenVisitor &visitor) -> llvm::Value * = 0;

  auto getType() const -> Type { return type; }
  auto setType(Type type) -> void { this->type = type; }

  virtual ~AstNode() = default;
};

} // namespace fern

#endif
