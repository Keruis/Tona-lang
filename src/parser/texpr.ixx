export module tona.expr;

import std;

export namespace Tona {

  enum class ExprNodeType : std::uint8_t {
    ENT_NONE,
    ENT_LITERAL_INT,
    ENT_LITERAL_FLOAT,
    ENT_LITERAL_STRING,
    ENT_VARIABLE
  };

}