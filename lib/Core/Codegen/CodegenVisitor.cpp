#include "Codegen/CodegenVisitor.hpp"
#include "Sema/Type.hpp"
#include "AST/Nodes.hpp"
#include "Errors/Context.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Verifier.h"

namespace fern {

auto CodegenVisitor::visit(ProgramNode &node) -> void {
  for (auto &ext: node.getExterns()) {
    visit(*ext);
  }

  for (auto &func: node.getFunctions()) {
    visit(*func);
  }
}

auto CodegenVisitor::visit(Function &node) -> void {
  llvm::Function *func = ctx.getModule().getFunction(node.getName());

  if (!func) {
    func = node.getProto()->codegen(*this);
  }

  if (!func) {
    return;
  }

  if (!func->empty()) {
    ctx.recordError("function already defined", node.getProto()->getLocation());
    return;
  }

  currentFunction = func;

  varValueTable.incScope();
  for (auto &arg: func->args()) {
    varValueTable.insert(arg.getName().data(), &arg);
  }

  node.getBody()->codegen(*this);
  if (verifyFunction(*func, &llvm::errs())) {
    ctx.recordError("llvm function verification failed", node.getProto()->getLocation());
    func->print(llvm::errs());
    func->eraseFromParent();
  }

  varValueTable.decScope();
}

auto CodegenVisitor::visit(ExternDef &node) -> void {
  llvm::Function *func = node.getProto()->codegen(*this);

  if (!func) {
    return;
  }
}

auto CodegenVisitor::visit(fern::Type &type) -> llvm::Type * {
  llvm::Type *llvmType = nullptr;
  switch (type.getKind()) {
  case TypeKind::Void:
    llvmType = llvm::Type::getVoidTy(ctx.getLLVMContext());
    break;
  case TypeKind::Bool:
    llvmType = llvm::Type::getInt1Ty(ctx.getLLVMContext());
    break;
  case TypeKind::Int:
    llvmType = llvm::Type::getInt32Ty(ctx.getLLVMContext());
    break;
  case TypeKind::Float:
    llvmType = llvm::Type::getFloatTy(ctx.getLLVMContext());
    break;
  case TypeKind::Char:
    llvmType = llvm::Type::getInt8Ty(ctx.getLLVMContext());
    break;
  case TypeKind::Str:
    llvmType = llvm::Type::getInt8PtrTy(ctx.getLLVMContext());
    break;
  case TypeKind::Invalid:
    llvmType = nullptr;
    break;
  }

  if (type.getReferenceDepth() > 0) {
    llvmType = llvm::PointerType::get(llvmType, type.getReferenceDepth());
  }

  return llvmType;
}

auto CodegenVisitor::visit(Prototype &node) -> llvm::Function * {
  std::vector<llvm::Type *> argTypes;
  for (auto &arg: node.getArgs()) {
    argTypes.push_back(arg->type.codegen(*this));
  }

  llvm::FunctionType *funcType = llvm::FunctionType::get(node.getReturnType().codegen(*this), argTypes, false);
  llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.getName(), &ctx.getModule());

  if (func->getName() != node.getName()) {
    func->eraseFromParent();
    ctx.recordError("function name already in use", node.getLocation());
    return nullptr;
  }

  if (func->arg_size() != node.getArgs().size()) {
    func->eraseFromParent();
    ctx.recordError("function argument count mismatch", node.getLocation());
    return nullptr;
  }

  usize i = 0;
  for (auto &arg: func->args()) {
    arg.setName(node.getArgs()[i++]->name);
  }

  return func;
}

auto CodegenVisitor::visit(BinaryNode &node) -> llvm::Value * {
  llvm::Value *rhs = node.getRhs()->codegen(*this);
  if (!rhs) {
    return nullptr;
  }

  llvm::Value *lhs = node.getLhs()->codegen(*this);
  if (!lhs) {
    return nullptr;
  }

  switch (node.getOp()) {
    case TokenKind::Equal:
    case TokenKind::ColonEqual:
      if (lhs->getType() != rhs->getType()) {
        ctx.recordError("cannot reassign variable with different type", node.getLocation());
        ctx.recordNote(fmt::format("variable `{}` expected type {}, got {}",
                                   node.getLhs()->as<VariableNode>()->getName(),
                                   node.getLhs()->getType().getTypeName(),
                                   node.getRhs()->getType().getTypeName()));
        return nullptr;
      }
      return ctx.getBuilder().CreateStore(rhs, varValueTable.lookup(node.getLhs()->as<VariableNode>()->getName()));
    case TokenKind::EqualEqual:
      return ctx.getBuilder().CreateICmpEQ(lhs, rhs, "eqtmp");
    case TokenKind::BangEqual:
      return ctx.getBuilder().CreateICmpNE(lhs, rhs, "neqtmp");
    case TokenKind::Plus:
      return ctx.getBuilder().CreateAdd(lhs, rhs, "addtmp");
    case TokenKind::Minus:
      return ctx.getBuilder().CreateSub(lhs, rhs, "subtmp");
    case TokenKind::Star:
      return ctx.getBuilder().CreateMul(lhs, rhs, "multmp");
    case TokenKind::Slash:
      return ctx.getBuilder().CreateSDiv(lhs, rhs, "divtmp");
    case TokenKind::Less:
      return ctx.getBuilder().CreateICmpSLT(lhs, rhs, "lttmp");
    case TokenKind::LessEqual:
      return ctx.getBuilder().CreateICmpSLE(lhs, rhs, "letmp");
    case TokenKind::Greater:
      return ctx.getBuilder().CreateICmpSGT(lhs, rhs, "gttmp");
    case TokenKind::GreaterEqual:
      return ctx.getBuilder().CreateICmpSGE(lhs, rhs, "getmp");
    default:
      ctx.recordError("invalid binary op", node.getLocation());
      return nullptr;
  }

  return nullptr;
}

auto CodegenVisitor::visit(UnaryNode &node) -> llvm::Value * {
  llvm::Value *rhs = node.getOperand()->codegen(*this);
  if (!rhs) {
    return nullptr;
  }

  switch (node.getOp().getKind()) {
  case TokenKind::Minus:
    return ctx.getBuilder().CreateNeg(rhs, "negtmp");
  case TokenKind::Bang:
    return ctx.getBuilder().CreateNot(rhs, "nottmp");
  default:
    ctx.recordError("invalid unary op", node.getLocation());
    return nullptr;
  }
}

auto CodegenVisitor::visit(IfNode &node) -> llvm::Value * {
  llvm::Value *cond = node.getCondition()->codegen(*this);
  if (!cond) {
    return nullptr;
  }

  llvm::Function *func = ctx.getBuilder().GetInsertBlock()->getParent();
  
  llvm::Value *thenBlockValue = node.getThenBlock()->codegen(*this);
  llvm::BasicBlock *thenBlock = ctx.getBuilder().GetInsertBlock();

  llvm::Value *elseBlockValue = nullptr;
  llvm::BasicBlock *elseBlock = nullptr;
  if (auto elseBlockNode = node.getElseBlock()) {
    elseBlockValue = elseBlockNode->codegen(*this);
    elseBlock = ctx.getBuilder().GetInsertBlock();
  }

  llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(ctx.getLLVMContext(), "ifcont", func);
  ctx.getBuilder().CreateCondBr(cond, thenBlock, elseBlock ? elseBlock : mergeBlock);
  ctx.getBuilder().SetInsertPoint(mergeBlock);

  if (thenBlockValue && elseBlockValue) {
    llvm::PHINode *phi = ctx.getBuilder().CreatePHI(thenBlockValue->getType(), 2, "iftmp");
    phi->addIncoming(thenBlockValue, thenBlock);
    phi->addIncoming(elseBlockValue, elseBlock);
    return phi;
  }

  return nullptr;
}

auto CodegenVisitor::visit(LetNode &node) -> llvm::Value * {
  llvm::Value *value = node.getValue()->codegen(*this);
  if (!value) {
    return nullptr;
  }

  varValueTable.insert(node.getName(), value);
  return value;
}

auto CodegenVisitor::visit(BlockNode &node) -> llvm::Value * {
  llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.getLLVMContext(), "block", currentFunction);
  ctx.getBuilder().SetInsertPoint(block);

  std::vector<llvm::Value *> values;
  for (auto &stmt: node.getNodes()) {
    values.push_back(stmt->codegen(*this));
  }

  return values.back() ? values.back() : nullptr;
}

auto CodegenVisitor::visit(SingleOpNode &node) -> llvm::Value * {
  if (node.getOp() == TokenKind::Return) {
    if (auto expr = node.getExpr()) {
      llvm::Value *value = expr->codegen(*this);
      if (!value) {
        return nullptr;
      }

      return ctx.getBuilder().CreateRet(value);
    } else {
      return ctx.getBuilder().CreateRetVoid();
    }
  }
  switch (node.getOp()) {
    case TokenKind::Continue:
      // return ctx.getBuilder().CreateBr(ctx.getContinueBlock());
    case TokenKind::Break:
      // return ctx.getBuilder().CreateBr(ctx.getBreakBlock());
    default:
      ctx.recordError("invalid single op", node.getLocation());
      return nullptr;
  }
}

auto CodegenVisitor::visit(CallNode &node) -> llvm::Value * {
  llvm::Function *func = ctx.getModule().getFunction(node.getCallee());
  if (!func) {
    ctx.recordError("function not found", node.getLocation());
    return nullptr;
  }

  if (func->arg_size() != node.getArgs().size()) {
    ctx.recordError("function argument count mismatch", node.getLocation());
    return nullptr;
  }

  std::vector<llvm::Value *> args;
  for (auto &arg: node.getArgs()) {
    args.push_back(arg->codegen(*this));
    if (!args.back()) {
      return nullptr;
    }
  }

  return ctx.getBuilder().CreateCall(func, args, "calltmp");
}

auto CodegenVisitor::visit(VariableNode &node) -> llvm::Value * {
  llvm::Value *value = varValueTable.lookup(node.getName());
  if (!value) {
    ctx.recordError("variable not found", node.getLocation());
    return nullptr;
  }

  // return ctx.getBuilder().CreateLoad(value, node.getName());
  return ctx.getBuilder().CreateLoad(node.getType().codegen(*this), value, node.getName());
}

auto CodegenVisitor::visit(SubscriptNode &node) -> llvm::Value * {
  llvm::Value *operand = node.getOperand()->codegen(*this);
  if (!operand) {
    return nullptr;
  }

  llvm::Value *index = node.getIndex()->codegen(*this);
  if (!index) {
    return nullptr;
  }

  return ctx.getBuilder().CreateGEP(node.getIndexedType().codegen(*this), operand, index, "subscripttmp");
}

auto CodegenVisitor::visit(BooleanNode &node) -> llvm::Value * {
  return llvm::ConstantInt::get(ctx.getLLVMContext(), llvm::APInt(1, node.getValue() ? 1 : 0, true));
}

auto CodegenVisitor::visit(NumberNode &node) -> llvm::Value * {
  if (node.getType() == Type::Int()) {
    return llvm::ConstantInt::get(ctx.getLLVMContext(), llvm::APInt(32, node.getValue(), 10));
  } else if (node.getType() == Type::Float()) {
    return llvm::ConstantFP::get(ctx.getLLVMContext(), llvm::APFloat(llvm::APFloat::IEEEdouble(), node.getValue()));
  } else {
    ctx.recordError("invalid number type", node.getLocation());
    return nullptr;
  }
}

auto CodegenVisitor::visit(StringNode &node) -> llvm::Value * {
  return ctx.getBuilder().CreateGlobalStringPtr(node.getValue());
}

}
