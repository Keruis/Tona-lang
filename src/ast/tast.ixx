module tona.ast;

import std;

import tona.byte;
import tona.expr;
import tona.stmt;
import tona.arena;

namespace Tona {

  void ASTBuild::stmt_begin(StmtNodeType type) {
    auto _ = ast.data.create<StmtNodeType>(type);
  }

  void ASTBuild::stmt_end() {

  }

  template <typename T>
  T* ASTBuild::push_raw(T&& val) {
    return ast.data.create<T>(val);
  }
}