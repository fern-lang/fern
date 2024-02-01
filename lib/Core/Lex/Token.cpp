#include "Parse/Lex/Token.hpp"

namespace fern {

auto tokenKindToString(TokenKind kind) -> std::string {
  switch (kind) {
  case TokenKind::LParen:
    return "LParen";
  case TokenKind::RParen:
    return "RParen";
  case TokenKind::LBrace:
    return "LBrace";
  case TokenKind::RBrace:
    return "RBrace";
  case TokenKind::LBracket:
    return "LBracket";
  case TokenKind::RBracket:
    return "RBracket";
  case TokenKind::Comma:
    return "Comma";
  case TokenKind::Dot:
    return "Dot";
  case TokenKind::Colon:
    return "Colon";
  case TokenKind::Semicolon:
    return "Semicolon";
  case TokenKind::Ref:
    return "Ref";

  case TokenKind::Plus:
    return "Plus";
  case TokenKind::Minus:
    return "Minus";
  case TokenKind::Star:
    return "Star";
  case TokenKind::Slash:
    return "Slash";
  case TokenKind::Arrow:
    return "Arrow";
  case TokenKind::Equal:
    return "Equal";
  case TokenKind::EqualEqual:
    return "EqualEqual";
  case TokenKind::ColonEqual:
    return "ColonEqual";
  case TokenKind::Bang:
    return "Bang";
  case TokenKind::BangEqual:
    return "BangEqual";
  case TokenKind::Less:
    return "Less";
  case TokenKind::LessEqual:
    return "LessEqual";
  case TokenKind::Greater:
    return "Greater";
  case TokenKind::GreaterEqual:
    return "GreaterEqual";
  
  case TokenKind::Ident:
    return "Ident";
  case TokenKind::String:
    return "String";
  case TokenKind::Char:
    return "Char";
  case TokenKind::Integer:
    return "Integer";
  case TokenKind::Float:
    return "Float";
  case TokenKind::Bool:
    return "Bool";

  case TokenKind::Let:
    return "Let";
  case TokenKind::If:
    return "If";
  case TokenKind::Else:
    return "Else";
  case TokenKind::For:
    return "For";
  case TokenKind::While:
    return "While";
  case TokenKind::Return:
    return "Return";
  case TokenKind::Break:
    return "Break";
  case TokenKind::Continue:
    return "Continue";
  case TokenKind::True:
    return "True";
  case TokenKind::False:
    return "False";
  case TokenKind::Nil:
    return "Nil";
  case TokenKind::Func:
    return "Func";
  case TokenKind::Extern:
    return "Extern";
  
  case TokenKind::Eof:
    return "Eof";
  case TokenKind::Invalid:
    return "Invalid";
  case TokenKind::Comment:
    return "Comment";
  
  default:
    return "Unknown";
  }
}

}
