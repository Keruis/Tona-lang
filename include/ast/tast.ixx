export module tona.ast;

import std;

import tona.byte;
import tona.arena;

export namespace Tona {

  struct AST {
    Arena data;
    std::size_t file_idx;
  };

  class ASTBuild {
    public:
      ASTBuild(AST& ast)
        : ast(ast)
      {}

    template <typename T>
    T* push_raw(T&& val) {
      return ast.data.create<T>(std::move(val));
    }

    private:
      AST& ast;
  };

}