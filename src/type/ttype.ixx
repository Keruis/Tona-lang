export module tona.type;

import std;

export namespace Tona {

  enum class TypeKind {

  };

  enum ModifierFlags : std::uint8_t {
    MF_NONE   = 0,
    MF_CONST  = 1 << 0,
    MF_STATIC = 1 << 1,
  };

}