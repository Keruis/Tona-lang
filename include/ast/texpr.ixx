export module tona.expr;

import std;

import tona.ast;
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

      ExprBase() = default;
      constexpr ExprBase(const char* loc) noexcept
        : loc(loc)
      {}
    };
    
    struct LiteralExpr : public ExprBase {
      std::uint64_t val;

      LiteralExpr() = default;
      constexpr LiteralExpr(const char* loc, std::uint64_t val) noexcept
        : ExprBase(loc), val(val)
      {}

      static void operator()(ASTBuild& build, const char* loc, std::uint64_t val) {
        build.push_raw(LiteralExpr{
          loc,
          val
        });
      }
    };

    struct VariableExpr : public ExprBase {
      StringView name;

      VariableExpr() = default;
      constexpr VariableExpr(const char* loc, StringView name) noexcept
        : ExprBase(loc), name(name)
      {}

      static void operator()(ASTBuild& build, const char* loc, StringView name) {
        build.push_raw(VariableExpr{
          loc,
          name
        });
      }
    };

    // [UnaryExpr][val]
    struct UnaryExpr : public ExprBase {
      TokenType op;
      ExprNodeType val;

      UnaryExpr() = default;
      constexpr UnaryExpr(const char* loc, TokenType op, ExprNodeType val_type) noexcept
        : ExprBase(loc), op(op), val(val_type)
      {}

    };

    struct BinaryExpr : public ExprBase {
      ExprNodeType op;
      ExprNodeType lval;
      ExprNodeType rval;
    };

  }

}