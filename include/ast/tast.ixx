export module tona.ast;

import std;

import tona.byte;
import tona.expr;
import tona.stmt;
import tona.arena;

export namespace Tona {

  struct AST {
    Arena data;
  };

  class ASTBuild {
    public:
      ASTBuild(AST& ast)
        : ast(ast)
      {}

      void stmt_begin(StmtNodeType type);
      void stmt_end();

      template <typename T>
      T* push_raw(T&& val);

    private:
      AST& ast;
  };

}