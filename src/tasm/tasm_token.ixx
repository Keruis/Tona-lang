export module tona.asm_token;

import std;

import tona.opcode;

export namespace Tona {

  enum class AsmTokenType : std::uint8_t {
    ATT_DATA,
    ATT_TEXT,
    ATT_LABEL,
    ATT_OPCODE,
    ATT_REG,
    ATT_IMM,
  };

  struct AsmToken {
    AsmTokenType type;
    union {
      std::string_view text;
      std::size_t id;
      OpCode op;
    };
  };

}