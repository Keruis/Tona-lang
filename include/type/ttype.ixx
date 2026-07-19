export module tona.type;

import std;

export namespace Tona {

  enum class TypeKind {

  };

  enum StorageSpecifiers : std::uint8_t {
    SS_NONE = 0,
    SS_STATIC = 1 << 0
  };

  enum TypeQualifiers : std::uint8_t {
    TQ_NONE = 0,
    TQ_CONST = 1 << 0,
    TQ_IMME = 1 << 1
  };

}