#include "Parse/Parser.hpp"
#include "llvm/Support/raw_ostream.h"

namespace fern {

auto Parser::parse() -> bool {
  while (!tokens.isEof()) {

    switch (tokens.peek()->getKind()) {
    case TokenKind::Func: {
      auto func = parseFunction();
      if (!func) {
        return false;
      }

      ctx.addFunction(func);
      break;
    }
    case TokenKind::Extern: {
      auto ext = parseExternDef();
      if (!ext) {
        return false;
      }

      ctx.addExtern(ext);
      break;
    }
    default:
      ctx.recordError("unexpected token in top scope", tokens.peek()->getLocation());
      return false;
    }
  }

  return ctx.getErrors().empty();
}

auto Parser::parseFunctionPrototype() -> std::shared_ptr<Prototype> {
  if (tokens.peek()->getKind() != TokenKind::Func) {
    ctx.recordError("unexpected token, expected `func`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();

  if (tokens.peek()->getKind() != TokenKind::Ident) {
    ctx.recordError("unexpected token, expected identifier", tokens.peek()->getLocation());
    return nullptr;
  }
  auto funcName = *tokens.next();

  if (tokens.peek()->getKind() != TokenKind::LParen) {
    ctx.recordError("unexpected token, expected `(`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();

  std::vector<std::shared_ptr<PrototypeArg>> args;
  while (tokens.peek()->getKind() != TokenKind::RParen) {
    auto argName = Token::makeInvalid();
    if (tokens.peek()->getKind() != TokenKind::Ident) {
      ctx.recordError("unexpected token, expected identifier", tokens.peek()->getLocation());
      return nullptr;
    }
    argName = *tokens.next();

    // parse type annotation
    if (tokens.peek()->getKind() != TokenKind::Colon) {
      ctx.recordError("unexpected token, expected `:`", tokens.peek()->getLocation());
      return nullptr;
    }
    tokens.next();

    if (tokens.peek()->getKind() != TokenKind::Ident) {
      ctx.recordError("unexpected token, expected identifier", tokens.peek()->getLocation());
      return nullptr;
    }
    auto argType = *tokens.next();
    
    args.emplace_back(std::make_shared<PrototypeArg>(argName.getLexeme(), argType));

    if (tokens.peek()->getKind() == TokenKind::Comma) {
      tokens.next();
    } else if (tokens.peek()->getKind() != TokenKind::RParen) {
      ctx.recordError("unexpected token, expected `,` or `)`", tokens.peek()->getLocation());
      return nullptr;
    }

    if (tokens.peek()->getKind() == TokenKind::RParen) {
      break;
    }
  }

  if (tokens.peek()->getKind() != TokenKind::RParen) {
    ctx.recordError("unexpected token, expected `)`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();


  Token retType = Token::makeInvalid();
  if (tokens.peek()->getKind() == TokenKind::Arrow) {
    tokens.next();

    if (tokens.peek()->getKind() != TokenKind::Ident) {
      ctx.recordError("unexpected token, expected identifier", tokens.peek()->getLocation());
      return nullptr;
    }
    retType = *tokens.next();
  }

  return std::make_shared<Prototype>(funcName.getLexeme(), args, retType);
}

auto Parser::parseFunction() -> std::shared_ptr<Function> {
  auto proto = parseFunctionPrototype();
  if (!proto) {
    return nullptr;
  }

  auto body = parseBlockExpr();
  if (!body) {
    return nullptr;
  }

  return std::make_shared<Function>(proto, body);
}

auto Parser::parseExternDef() -> std::shared_ptr<ExternDef> {
  auto loc = tokens.peek()->getLocation();
  
  if (tokens.peek()->getKind() != TokenKind::Extern) {
    ctx.recordError("unexpected token, expected `extern`", loc);
    return nullptr;
  }
  tokens.next();

  auto proto = parseFunctionPrototype();
  if (!proto) {
    ctx.recordError("failed to parse function prototype", loc);
    return nullptr;
  }

  if (tokens.peek()->getKind() != TokenKind::Semicolon) {
    ctx.recordError("unexpected token, expected `;`", loc);
    return nullptr;
  }
  tokens.next();

  return std::make_shared<ExternDef>(proto);
}

auto Parser::parsePrimary() -> std::shared_ptr<AstNode> {
  switch (tokens.peek()->getKind()) {
  case TokenKind::LBrace:
    return parseBlockExpr();
  case TokenKind::LParen:
    return parseParenExpr();
  case TokenKind::Ident:
    return parseIdentifierExpr();
  case TokenKind::Integer:
  case TokenKind::Float:
    return parseNumExpr();
  case TokenKind::String:
    return parseStringExpr();
  case TokenKind::True:
  case TokenKind::False:
    return parseBoolExpr();
  case TokenKind::Let:
    return parseLetExpr();
  case TokenKind::If:
    return parseIfExpr();
  case TokenKind::Return:
  case TokenKind::Continue:
  case TokenKind::Break:
    return parseSingleOpExpr();
  default:
    ctx.recordError("unexpected token", tokens.peek()->getLocation());
    return nullptr;
  }
}

auto Parser::parseBinOpRHS(int exprPrec, std::shared_ptr<AstNode> lhs) -> std::shared_ptr<AstNode> {
  auto loc = tokens.peek()->getLocation();

  while (true) {
    int tokPrec = getTokenPrec(tokens.peek()->getKind());

    if (tokPrec < exprPrec)
      return lhs;

    auto binOp = *tokens.next();

    auto rhs = parseUnary();
    if (!rhs) {
      return nullptr;
    }

    int nextPrec = getTokenPrec(tokens.peek()->getKind());
    if (tokPrec < nextPrec) {
      rhs = parseBinOpRHS(tokPrec + 1, rhs);
      if (!rhs) {
        return nullptr;
      }
    }

    lhs = std::make_shared<BinaryNode>(loc, binOp.getKind(), lhs, rhs);
  }
}

auto Parser::parseUnary() -> std::shared_ptr<AstNode> {
  if (tokens.peek()->getKind() != TokenKind::Minus && tokens.peek()->getKind() != TokenKind::Bang) {
    return parsePrimary();
  }

  auto op = *tokens.next();
  if (auto operand = parseUnary()) {
    return std::make_shared<UnaryNode>(op.getLocation(), op, operand);
  }

  return nullptr;
}

auto Parser::parseParenExpr() -> std::shared_ptr<AstNode> {
  auto loc = tokens.peek()->getLocation();

  if (tokens.peek()->getKind() != TokenKind::LParen) {
    ctx.recordError("unexpected token, expected `(`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();

  auto expr = parseExpr();
  if (!expr) {
    ctx.recordError("failed to parse parenthesis expression", loc);
    return nullptr;
  }

  if (tokens.peek()->getKind() != TokenKind::RParen) {
    ctx.recordError("expected closing `)` for expression", loc);
    return nullptr;
  }
  tokens.next();

  return expr;
}

auto Parser::parseIdentifierExpr() -> std::shared_ptr<AstNode> {
  auto ident = *tokens.next();

  if (tokens.peek()->getKind() != TokenKind::LParen) {
    return std::make_shared<VariableNode>(ident.getLocation(), ident.getLexeme());
  }
  tokens.next();

  std::vector<std::shared_ptr<AstNode>> args;
  while (tokens.peek()->getKind() != TokenKind::RParen) {
    auto expr = parseExpr();
    if (!expr) {
      ctx.recordError("failed to parse call argument", tokens.peek()->getLocation());
      return nullptr;
    }

    args.push_back(expr);

    if (tokens.peek()->getKind() == TokenKind::Comma) {
      tokens.next();
    } else if (tokens.peek()->getKind() != TokenKind::RParen) {
      ctx.recordError("expected `,` or `)`", tokens.peek()->getLocation());
      return nullptr;
    }

    if (tokens.peek()->getKind() == TokenKind::RParen) {
      break;
    }
  }

  if (tokens.peek()->getKind() != TokenKind::RParen) {
    ctx.recordError("expected closing `)`", ident.getLocation());
    return nullptr;
  }
  tokens.next();

  return std::make_shared<CallNode>(ident.getLocation(), ident.getLexeme(), args);
}

auto Parser::parseStringExpr() -> std::shared_ptr<AstNode> {
  auto str = *tokens.next();

  return std::make_shared<StringNode>(str.getLocation(), str.getLexeme());
}

auto Parser::parseNumExpr() -> std::shared_ptr<AstNode> {
  auto num = *tokens.next();

  return std::make_shared<NumberNode>(num.getLocation(), num.getLexeme(), num.getKind() == TokenKind::Float);
}

auto Parser::parseBoolExpr() -> std::shared_ptr<AstNode> {
  auto boolean = *tokens.next();
  return std::make_shared<BooleanNode>(boolean.getLocation(), boolean.getKind() == TokenKind::True);
}

auto Parser::parseLetExpr() -> std::shared_ptr<AstNode> {
  if (tokens.peek()->getKind() != TokenKind::Let) {
    ctx.recordError("unexpected token, expected `let`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();

  if (tokens.peek()->getKind() != TokenKind::Ident) {
    ctx.recordError("expected variable name after `let`", tokens.peek()->getLocation());
    return nullptr;
  }
  auto ident = *tokens.next();

  Token type = Token::makeInvalid();
  // if (tokens.peek()->getKind() == TokenKind::Colon) {
  //   tokens.next();

  //   if (tokens.peek()->getKind() != TokenKind::Ident) {
  //     ctx.recordError("expected valid type annotation", tokens.peek()->getLocation());
  //     return nullptr;
  //   }
  //   auto type = *tokens.next();
  // }

  if (tokens.peek()->getKind() != TokenKind::Equal) {
    ctx.recordError("expected `=`", tokens.peek()->getLocation());
    return nullptr;
  }
  tokens.next();

  auto value = parseExpr();
  if (!value) {
    ctx.recordError("failed to parse variable value", tokens.peek()->getLocation());
    return nullptr;
  }

  return std::make_shared<LetNode>(ident.getLocation(), ident.getLexeme(), type, value);
}

auto Parser::parseSingleOpExpr() -> std::shared_ptr<AstNode> {
  auto op = *tokens.next();

  if (op.getKind() == TokenKind::Return) {
    auto expr = parseExpr();
    if (!expr) {
      return nullptr;
    }

    return std::make_shared<SingleOpNode>(op.getLocation(), op.getKind(), expr);
  }

  return std::make_shared<SingleOpNode>(op.getLocation(), op.getKind(), nullptr);
}

auto Parser::parseExpr() -> std::shared_ptr<AstNode> {
  auto lhs = parseUnary();
  if (!lhs) {
    return nullptr;
  }

  return parseBinOpRHS(0, lhs);
}

auto Parser::parseIfExpr() -> std::shared_ptr<AstNode> {
  tokens.next();

  auto cond = parseExpr();
  if (!cond) {
    ctx.recordError("failed to parse if condition", tokens.peek()->getLocation());
    return nullptr;
  }

  if (tokens.peek()->getKind() != TokenKind::LBrace) {
    ctx.recordError("expected `{`", tokens.peek()->getLocation());
    return nullptr;
  }

  auto ifBlock = parseBlockExpr();
  if (!ifBlock) {
    return nullptr;
  }

  auto hasElse = false;
  std::shared_ptr<AstNode> elseBlock = nullptr;
  if (tokens.peek()->getKind() == TokenKind::Else) {
    tokens.next();

    if (tokens.peek()->getKind() != TokenKind::LBrace) {
      ctx.recordError("expected `{`", tokens.peek()->getLocation());
      return nullptr;
    }

    elseBlock = parseBlockExpr();
    if (!elseBlock) {
      return nullptr;
    }

    hasElse = true;
  }

  return std::make_shared<IfNode>(cond->getLocation(), cond, ifBlock, elseBlock);
}

auto Parser::parseBlockExpr() -> std::shared_ptr<AstNode> {
  auto loc = tokens.next()->getLocation();

  std::vector<std::shared_ptr<AstNode>> exprs;
  while (tokens.peek()->getKind() != TokenKind::RBrace) {
    auto expr = parseExpr();
    if (!expr) {
      return nullptr;
    }

    exprs.push_back(expr);

    if (expr->is<IfNode>() || expr->is<BlockNode>()) {
      continue;
    }
    
    if (tokens.peek()->getKind() == TokenKind::Semicolon) {
      tokens.next();
    } else if (tokens.peek()->getKind() != TokenKind::RBrace) {
      ctx.recordError("expected `;` or `}`",
                      tokens.peek()->getLocation());
      return nullptr;
    }
  }
  tokens.next();

  return std::make_shared<BlockNode>(loc, exprs);
}

}
