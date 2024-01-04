#ifndef Fern_Parse_Parser_hpp
#define Fern_Parse_Parser_hpp

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <memory>
#include "TokenIterator.hpp"
#include "../Errors/Context.hpp"
#include "Lex/Token.hpp"
#include "../AST/Nodes.hpp"

namespace fern {

class Parser {
  TokenIterator tokens;
  Context &ctx;

  std::map<TokenKind, int> binOpPrec = {
    {TokenKind::Equal, 1},
    {TokenKind::ColonEqual, 1},

    {TokenKind::EqualEqual, 2},
    {TokenKind::BangEqual, 2},

    {TokenKind::Less, 3},
    {TokenKind::LessEqual, 3},
    {TokenKind::Greater, 3},
    {TokenKind::GreaterEqual, 3},

    {TokenKind::Plus, 4},
    {TokenKind::Minus, 4},

    {TokenKind::Star, 5},
    {TokenKind::Slash, 5},
  };

public:
  Parser(const std::vector<Token> &tokens, Context &ctx) : tokens(std::move(tokens)), ctx(ctx) {}

  auto parse() -> bool;

private:
  auto getTokenPrec(TokenKind kind) -> int {
    auto it = binOpPrec.find(kind);
    if (it == binOpPrec.end()) {
      return -1;
    }

    return it->second;
  }

  auto parseFunctionPrototype() -> std::shared_ptr<Prototype>;
  auto parseFunction() -> std::shared_ptr<Function>;
  auto parseExternDef() -> std::shared_ptr<ExternDef>;

  auto parsePrimary() -> std::shared_ptr<AstNode>;

  auto parseNumExpr() -> std::shared_ptr<AstNode>;
  auto parseStringExpr() -> std::shared_ptr<AstNode>;
  auto parseBoolExpr() -> std::shared_ptr<AstNode>;

  auto parseParenExpr() -> std::shared_ptr<AstNode>;
  auto parseIdentifierExpr() -> std::shared_ptr<AstNode>;
  auto parseSingleOpExpr() -> std::shared_ptr<AstNode>;

  auto parseExpr() -> std::shared_ptr<AstNode>;
  auto parseUnary() -> std::shared_ptr<AstNode>;
  auto parseBinOpRHS(int exprPrec, std::shared_ptr<AstNode> lhs) -> std::shared_ptr<AstNode>;
  auto parseBlockExpr() -> std::shared_ptr<AstNode>;

  auto parseIfExpr() -> std::shared_ptr<AstNode>;
  auto parseLetExpr() -> std::shared_ptr<AstNode>;
};

}

#endif
