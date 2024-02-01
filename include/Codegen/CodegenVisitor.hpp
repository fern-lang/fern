#ifndef Fern_Codegen_CodegenVisitor_hpp
#define Fern_Codegen_CodegenVisitor_hpp

#include <memory>
#include <optional>
#include <string>
#include "../AST/SymbolTable.hpp"

#include "llvm/IR/Value.h"


namespace fern {

class Type;

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

class   CodegenVisitor {
public:
  CodegenVisitor(Context &ctx) : ctx(ctx) {}

  auto visit(ProgramNode &node) -> void;
  auto visit(Function &node) -> void;
  auto visit(ExternDef &node) -> void;
  auto visit(Type &type) -> llvm::Type *;
  auto visit(Prototype &node) -> llvm::Function *;

  auto visit(BinaryNode &node) -> llvm::Value *;
  auto visit(UnaryNode &node) -> llvm::Value *;
  auto visit(IfNode &node) -> llvm::Value *;
  auto visit(LetNode &node) -> llvm::Value *;
  auto visit(BlockNode &node) -> llvm::Value *;
  auto visit(SingleOpNode &node) -> llvm::Value *;
  auto visit(CallNode &node) -> llvm::Value *;
  auto visit(VariableNode &node) -> llvm::Value *;
  auto visit(SubscriptNode &node) -> llvm::Value *;

  auto visit(BooleanNode &node) -> llvm::Value *;
  auto visit(NumberNode &node) -> llvm::Value *;
  auto visit(StringNode &node) -> llvm::Value *;

private:
  Context &ctx;
  RawPtrSymbolTable<llvm::Value> varValueTable;
  llvm::Function *currentFunction = nullptr;
};

} // namespace fern

#endif
