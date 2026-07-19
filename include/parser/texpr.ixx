export module tona.expr;

import std;

import tona.token;
import tona.string;

export namespace Tona {

  enum class ExprNodeType : std::uint8_t {
    ENT_NONE,
    ENT_LITERAL_INT,
    ENT_LITERAL_FLOAT,
    ENT_LITERAL_STRING,
    ENT_VARIABLE,
    ENT_UNARY,
    ENT_BINARY
  };

  namespace Expr {

    struct ExprBase {
      const char* loc;
      // type
    };
    
    struct LiteralExpr : public ExprBase {
      std::uint64_t val;
    };

    struct VariableExpr : public ExprBase {
      StringView name;
    };

    struct UnaryExpr : public ExprBase {
      ExprNodeType op;
      ExprNodeType val;
    };

    struct BinaryExpr : public ExprBase {
      ExprNodeType op;
      ExprNodeType lval;
      ExprNodeType rval;
    };

  }

}