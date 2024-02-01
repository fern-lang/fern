#ifndef Fern_Sema_TypeVisitor_hpp
#define Fern_Sema_TypeVisitor_hpp

#include <optional>
#include <string>
#include "../AST/SymbolTable.hpp"
#include "Type.hpp"

namespace fern {

class Context;

class ProgramNode;

class Function;
class ExternDef;
class Prototype;

class BinaryNode;
class UnaryNode;
class IfNode;
class LetNode;
class BlockNode;
class SingleOpNode;
class CallNode;
class VariableNode;
class SubscriptNode;

class BooleanNode;
class NumberNode;
class StringNode;

struct FunctionType {
  std::vector<Type> paramTypes;
  Type returnType;

  FunctionType(const std::vector<Type> &paramTypes, const Type &returnType) :
      paramTypes(paramTypes), returnType(returnType) {}
  FunctionType(const FunctionType &other) :
      paramTypes(other.paramTypes), returnType(other.returnType) {}
  
  auto operator=(const FunctionType &other) -> FunctionType& {
    paramTypes = other.paramTypes;
    returnType = other.returnType;
    return *this;
  }

  auto getArity() const -> usize { return paramTypes.size(); }
};

struct CheckContext {
  std::optional<FunctionType> currentFunction;
  bool inBreakableScope = false; // continue and break
};

class TypeVisitor {
public:
  TypeVisitor(Context &ctx) : ctx(ctx) {}

  auto visit(ProgramNode &node) -> void;

  auto visit(Function &node) -> void;
  auto visit(ExternDef &node) -> void;
  auto visit(Prototype &node) -> void;

  auto visit(BinaryNode &node) -> void;
  auto visit(UnaryNode &node) -> void;
  auto visit(IfNode &node) -> void;
  auto visit(LetNode &node) -> void;
  auto visit(BlockNode &node) -> void;
  auto visit(SingleOpNode &node) -> void;
  auto visit(CallNode &node) -> void;
  auto visit(VariableNode &node) -> void;
  auto visit(SubscriptNode &node) -> void;

  auto visit(BooleanNode &node) -> void;
  auto visit(NumberNode &node) -> void;
  auto visit(StringNode &node) -> void;

  auto lookupFunction(const std::string &name) -> std::optional<FunctionType> {
    auto found = funcSymbolTable.find(name);
    if (found != funcSymbolTable.end()) {
      return found->second;
    }
    return std::nullopt;
  }

private:
  Context &ctx;
  CheckContext checkCtx;
  SymbolTable<Type> varSymbolTable;
  std::unordered_map<std::string, FunctionType> funcSymbolTable;
};

} // namespace fern

#endif
