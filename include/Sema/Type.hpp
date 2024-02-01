#ifndef Fern_Sema_Type_hpp
#define Fern_Sema_Type_hpp

#include <Roots/_defines.hpp>
#include <optional>
#include <string>
#include "../Parse/Lex/Token.hpp"
#include "llvm/IR/Type.h"
#include "../Codegen/CodegenVisitor.hpp"

namespace fern {

enum class TypeKind { Void, Bool, Int, Float, Char, Str, Invalid };

static auto nameForTypeKind(TypeKind kind) -> std::string {
  switch (kind) {
    case TypeKind::Void:
      return "void";
    case TypeKind::Bool:
      return "bool";
    case TypeKind::Int:
      return "int";
    case TypeKind::Float:
      return "float";
    case TypeKind::Char:
      return "char";
    case TypeKind::Str:
      return "str";
    case TypeKind::Invalid:
      return "invalid";
  }
}

// TODO: Add support for type decay/promotion (e.g. int -> float, char -> int, etc.)
class Type {
  usize referenceDepth_;
  TypeKind kind_;

public:
  Type(TypeKind kind) : kind_(kind), referenceDepth_(0) {}
  Type(const Type &other) : kind_(other.kind_), referenceDepth_(other.referenceDepth_) {}
  Type(Type &&other) : kind_(other.kind_), referenceDepth_(other.referenceDepth_) {}

  auto operator=(const Type &other) -> Type & {
    kind_ = other.kind_;
    referenceDepth_ = other.referenceDepth_;
    return *this;
  }

  auto operator=(Type &&other) -> Type & {
    kind_ = other.kind_;
    referenceDepth_ = other.referenceDepth_;
    return *this;
  }

  auto operator==(const Type &other) const -> bool {
    return kind_ == other.kind_ && referenceDepth_ == other.referenceDepth_;
  }

  auto operator!=(const Type &other) const -> bool { return !(*this == other); }

  auto operator==(const TypeKind &other) const -> bool { return kind_ == other; }

  static auto Int() -> Type { return Type(TypeKind::Int); }
  static auto Float() -> Type { return Type(TypeKind::Float); }
  static auto Char() -> Type { return Type(TypeKind::Char); }
  static auto Str() -> Type { return Type(TypeKind::Str); }
  static auto Void() -> Type { return Type(TypeKind::Void); }
  static auto Bool() -> Type { return Type(TypeKind::Bool); }
  static auto PointerTo(Type type) -> Type {
    type.referenceDepth_ += 1;
    return type;
  }
  static auto Invalid() -> Type { return Type(TypeKind::Invalid); }

  auto getTypeName() const -> const std::string {
    std::string name;

    for (usize i = 0; i < referenceDepth_; ++i) {
      name += "&";
    }

    name += nameForTypeKind(kind_);

    return name;
  }

  auto ref() -> Type & {
    referenceDepth_ += 1;
    return *this;
  }

  auto deref() -> Type & {
    referenceDepth_ -= 1;
    return *this;
  }

  auto setRefDepth(usize depth) -> void { referenceDepth_ = depth; }

  auto isInvalid() const -> bool { return kind_ == TypeKind::Invalid; }
  auto isIndexable() const -> bool {
    return kind_ == TypeKind::Str || referenceDepth_ > 0;
  }

  auto codegen(CodegenVisitor &visitor) -> llvm::Type * { return visitor.visit(*this); }


  auto getReferenceDepth() const -> usize { return referenceDepth_; }
  auto getKind() const -> TypeKind { return kind_; }
};

} // namespace fern

#endif
