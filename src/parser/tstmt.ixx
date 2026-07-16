export module tona.stmt;

import std;

import tona.string;
import tona.type;
import tona.expr;

export namespace Tona {

  enum class StmtNodeType : std::uint8_t {
    SNT_VAR_DECL
  };

  namespace Stmt {
    
    // [StmtType][ExprStmt][expr]
    struct ExprStmt {
      ExprNodeType expr;
    };

    // [StmtType][DeclStmt][init]
    struct DeclStmt {
      StorageSpecifiers ss;
      TypeQualifiers tq;
      StringView name;
      ExprNodeType init;
    };

  }

}