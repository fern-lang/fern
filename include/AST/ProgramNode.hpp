#ifndef Fern_Ast_ProgramNode_hpp
#define Fern_Ast_ProgramNode_hpp

#include "../Sema/TypeVisitor.hpp"
#include "AstNode.hpp"
#include "ExternDef.hpp"
#include "Function.hpp"
#include "llvm/IR/Value.h"

namespace fern {

class ProgramNode : public AstNode {
  std::vector<std::shared_ptr<ExternDef>> externs;
  std::vector<std::shared_ptr<Function>> functions;

public:
  ProgramNode() : AstNode(SourceLocation()) {}

  auto addExtern(std::shared_ptr<ExternDef> ext) -> void { externs.push_back(ext); }

  auto addFunction(std::shared_ptr<Function> func) -> void { functions.push_back(func); }

  auto getExterns() const -> const std::vector<std::shared_ptr<ExternDef>> & {
    return externs;
  }

  auto getFunctions() const -> const std::vector<std::shared_ptr<Function>> & {
    return functions;
  }

  auto getFunctionsMutable() -> std::vector<std::shared_ptr<Function>> & {
    return functions;
  }

  auto getExternsMutable() -> std::vector<std::shared_ptr<ExternDef>> & {
    return externs;
  }

  auto getFunction(const std::string &name) const -> std::shared_ptr<Function> {
    for (auto &func: functions) {
      if (func->getName() == name) {
        return func;
      }
    }
    return nullptr;
  }

  auto getExtern(const std::string &name) const -> std::shared_ptr<ExternDef> {
    for (auto &ext: externs) {
      if (ext->getName() == name) {
        return ext;
      }
    }
    return nullptr;
  }

  auto print(llvm::raw_fd_ostream &out, usize indent) const -> void override {
    out.indent(indent) << "ProgramNode\n";
    out.indent(indent + 2) << "Externs:\n";
    for (auto &ext: externs) {
      ext->print(out, indent + 4);
    }
    out.indent(indent + 2) << "Functions:\n";
    for (auto &func: functions) {
      func->print(out, indent + 4);
    }
  }

  auto typeCheck(TypeVisitor &visitor) -> void override {
    for (auto &ext: externs) {
      ext->typeCheck(visitor);
    }

    for (auto &func: functions) {
      func->typeCheck(visitor);
    }
  }

  auto codegen(CodegenVisitor &visitor) -> llvm::Value * override {
    for (auto &ext: externs) {
      ext->codegen(visitor);
    }

    for (auto &func: functions) {
      func->codegen(visitor);
    }

    return nullptr;
  }
};

} // namespace fern

#endif
