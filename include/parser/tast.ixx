export module tona.ast;

import std;

import tona.byte;
import tona.expr;
import tona.stmt;
import tona.arena;

export namespace Tona {

  struct AST {
    Arena data;

    template <typename T>
    T* push_raw(T&& val) {
      return data.create<T>(val);
    }
  };

}