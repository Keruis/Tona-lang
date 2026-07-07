export module tona.ast;

import std;

import tona.type;

export namespace Tona {

  enum class ExprNodeType : std::uint8_t {
    ENT_NONE,
    ENT_LITERAL_INT,
    ENT_LITERAL_FLOAT,
    ENT_LITERAL_STRING,
    ENT_VARIABLE
  };

  enum class StmtNodeType : std::uint8_t {
    SNT_VAR_DECL
  };

  struct VarDeclNode {
    StmtNodeType type = StmtNodeType::SNT_VAR_DECL;
    ModifierFlags mf  = ModifierFlags::MF_NONE;
    ExprNodeType init = ExprNodeType::ENT_NONE;
  };
  
}