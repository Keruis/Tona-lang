export module tona.asm_token;

import std;

import tona.opcode;

export namespace Tona {

  enum class AsmTokenType : std::uint8_t {
    ATT_PUNCTUATORS_COLON, // :
    ATT_OPERATORS_LT, // <
    ATT_OPERATORS_GT, // >
    ATT_PUNCTUATORS_LBRACE, // {
    ATT_PUNCTUATORS_RBRACE, // }
    ATT_PUNCTUATORS_LPAREN, // (
    ATT_PUNCTUATORS_RPAREN, // )
    ATT_IDENTIFIER,
    ATT_STRING,
    ATT_OPCODE,
    ATT_IMM,
  };

  struct AsmToken {
    AsmTokenType type;
    union {
      std::string_view text;
      std::size_t num;
      OpCode op;
    };
  };

}