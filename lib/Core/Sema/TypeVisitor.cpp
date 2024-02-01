#include "Sema/TypeVisitor.hpp"
#include "AST/Nodes.hpp"
#include "Errors/Context.hpp"

namespace fern {

// only generate stubs for the functions we need to implement

auto TypeVisitor::visit(ProgramNode &node) -> void {
  for (auto &ext: node.getExternsMutable()) {
    ext->typeCheck(*this);
  }

  for (auto &func: node.getFunctionsMutable()) {
    func->typeCheck(*this);
  }
}

auto TypeVisitor::visit(Function &node) -> void {
  node.getProto()->typeCheck(*this);

  checkCtx.currentFunction = *lookupFunction(node.getName());

  varSymbolTable.incScope();
  for (auto &param: node.getProto()->getArgs()) {
    varSymbolTable.insert(param->name, std::make_shared<Type>(param->type));
  }

  node.getBody()->typeCheck(*this);
  varSymbolTable.decScope();

  checkCtx.currentFunction = std::nullopt;
}

auto TypeVisitor::visit(ExternDef &node) -> void { node.getProto()->typeCheck(*this); }

auto TypeVisitor::visit(Prototype &node) -> void {
  auto duplicate = lookupFunction(node.getName());
  if (duplicate) {
    ctx.recordError("duplicate function name", node.getLocation());
  }

  funcSymbolTable.emplace(node.getName(),
                          FunctionType(node.getArgTypes(), node.getReturnType()));
}

auto TypeVisitor::visit(BinaryNode &node) -> void {
  node.getLhs()->typeCheck(*this);
  node.getRhs()->typeCheck(*this);

  switch (node.getOp()) {
    case TokenKind::Equal:
    case TokenKind::ColonEqual:
      if (node.getLhs()->getType() != node.getRhs()->getType()) {
        ctx.recordError("cannot reassign variable with different type", node.getLocation());
        ctx.recordNote(fmt::format("variable `{}` expected type {}, got {}",
                                   node.getLhs()->as<VariableNode>()->getName(),
                                   node.getLhs()->getType().getTypeName(),
                                   node.getRhs()->getType().getTypeName()));
      }
      node.setType(node.getLhs()->getType());
      break;
    case TokenKind::EqualEqual:
    case TokenKind::BangEqual:
      if (node.getLhs()->getType() != node.getRhs()->getType()) {
        ctx.recordError("incorrect comparison type", node.getLocation());
      }
      node.setType(Type::Bool());
      break;
    case TokenKind::Less:
    case TokenKind::LessEqual:
    case TokenKind::Greater:
    case TokenKind::GreaterEqual:
      if (node.getLhs()->getType() != node.getRhs()->getType()) {
        ctx.recordError("incorrect comparison type", node.getLocation());
      }
      node.setType(Type::Bool());
      break;
    case TokenKind::Plus:
    case TokenKind::Minus:
    case TokenKind::Star:
    case TokenKind::Slash:
      if (node.getLhs()->getType() != Type::Int() &&
          node.getLhs()->getType() != Type::Float()) {
        ctx.recordError("incorrect arithmetic type", node.getLocation());
        ctx.recordNote(fmt::format("LHS is {}, RHS is {}",
                                   node.getLhs()->getType().getTypeName(),
                                   node.getRhs()->getType().getTypeName()));
        return;
      }

      if (node.getLhs()->getType() != node.getRhs()->getType()) {
        ctx.recordError("incorrect arithmetic type", node.getLocation());
        ctx.recordNote(fmt::format("LHS is {} while RHS is {}",
                                   node.getLhs()->getType().getTypeName(),
                                   node.getRhs()->getType().getTypeName()));
      }

      if (node.getOp() == TokenKind::Slash) {
        /*
          int / int = float (or int depending on parity)
          int / float = float
          float / int = float
          float / float = float
         */
        node.setType(Type::Float());
      } else {
        node.setType(node.getLhs()->getType());
      }
      break;
    default:
      ctx.recordError("unknown binary operator", node.getLocation());
      break;
  }
}

auto TypeVisitor::visit(UnaryNode &node) -> void {
  node.getOperand()->typeCheck(*this);

  if (node.getOp() == TokenKind::Minus) {
    if (node.getOperand()->getType() != Type::Int() &&
        node.getOperand()->getType() != Type::Float()) {
      ctx.recordError("incorrect operand type, expected int or float",
                      node.getLocation());
    }
    node.setType(node.getOperand()->getType());
  } else if (node.getOp() == TokenKind::Bang) {
    if (node.getOperand()->getType() != Type::Bool()) {
      ctx.recordError("incorrect operand type, expected bool", node.getLocation());
    }
    node.setType(node.getOperand()->getType());
  } else {
    ctx.recordError("unknown unary operator", node.getLocation());
  }
}

auto TypeVisitor::visit(IfNode &node) -> void {
  node.getCondition()->typeCheck(*this);
  if (node.getCondition()->getType() != Type::Bool()) {
    ctx.recordError("if condition must be a boolean", node.getLocation());
  }

  node.getThenBlock()->typeCheck(*this);

  if (node.hasElseBlock()) {
    node.getElseBlock()->typeCheck(*this);
  }

  if (node.hasElseBlock()) {
    if (node.getThenBlock()->getType() != node.getElseBlock()->getType()) {
      ctx.recordError("if/else blocks must have the same type", node.getLocation());
    }
    node.setType(node.getThenBlock()->getType());
  } else {
    node.setType(node.getThenBlock()->getType());
  }
}

auto TypeVisitor::visit(LetNode &node) -> void {
  auto value = node.getValue();
  value->typeCheck(*this);

  if (node.getTypeAnnotation()) {
    if (node.getTypeAnnotation().value() != value->getType()) {
      ctx.recordError("incorrect type annotation", node.getLocation());
    }
  }

  auto duplicate = varSymbolTable.lookup(node.getName());
  if (duplicate) {
    ctx.recordError("duplicate variable name", node.getLocation());
  }

  varSymbolTable.insert(node.getName(), std::make_shared<Type>(value->getType()));
  node.setType(value->getType());
}

auto TypeVisitor::visit(BlockNode &node) -> void {
  varSymbolTable.incScope();

  for (auto &stmt: node.getNodes()) {
    stmt->typeCheck(*this);
  }

  auto lastStmt = node.getNodes().back();
  node.setType(lastStmt->getType());

  varSymbolTable.decScope();
}

auto TypeVisitor::visit(SingleOpNode &node) -> void {
  if (node.getOp() == TokenKind::Return) {
    if (checkCtx.currentFunction) {
      node.getExpr()->typeCheck(*this);

      if (checkCtx.currentFunction->returnType != node.getExpr()->getType()) {
        ctx.recordError("incorrect return type", node.getExpr()->getLocation());
        ctx.recordNote(fmt::format("type is {}", node.getExpr()->getType().getTypeName()));
      }
    } else {
      ctx.recordError("return statement outside of function", node.getLocation());
    }
    node.setType(checkCtx.currentFunction->returnType);
  } else {
    // break/continue
    if (!checkCtx.inBreakableScope) {
      ctx.recordError("break/continue statement outside of loop", node.getLocation());
    }
    node.setType(Type::Void());
  }
}

auto TypeVisitor::visit(CallNode &node) -> void {
  auto func = lookupFunction(node.getCallee());
  if (!func) {
    ctx.recordError("unknown function name", node.getLocation());
    return;
  }

  if (func->getArity() != node.getArgs().size()) {
    ctx.recordError("incorrect number of arguments", node.getLocation());
    return;
  }

  for (usize i = 0; i < func->getArity(); ++i) {
    if (func->paramTypes[i] != node.getArgs()[i]->getType()) {
      ctx.recordError("incorrect argument type", node.getLocation());
      return;
    }
  }

  node.setType(func->returnType);
}

auto TypeVisitor::visit(VariableNode &node) -> void {
  auto var = varSymbolTable.lookup(node.getName());
  if (!var) {
    ctx.recordError("unknown variable name", node.getLocation());
    return;
  }

  node.setType(*var);
}

auto TypeVisitor::visit(SubscriptNode &node) -> void {
  node.getOperand()->typeCheck(*this);
  node.getIndex()->typeCheck(*this);

  if (!node.getOperand()->getType().isIndexable()) {
    ctx.recordError("operand is not indexable", node.getOperand()->getLocation());
    return;
  }

  if (node.getIndex()->getType() != Type::Int()) {
    ctx.recordError("index must be an integer", node.getIndex()->getLocation());
    return;
  }

  node.setType(node.getOperand()->getType().deref());
}

auto TypeVisitor::visit(BooleanNode &node) -> void {
  // noop
}

auto TypeVisitor::visit(NumberNode &node) -> void {
  // noop
}

auto TypeVisitor::visit(StringNode &node) -> void {
  // noop
}

} // namespace fern
