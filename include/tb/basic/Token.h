#ifndef __TOKEN__H__
#define __TOKEN__H__

#include "TokenKind.h"
#include "Location.h"

#include <string>
#include <string_view>

namespace tb::token {

constexpr static std::uint8_t double_operator_offset = 93;
constexpr static std::uint8_t suffix_offset = 1;

enum class TokenType : std::uint8_t {
  IDENTIFIER = 0,

  OPERATORS_NOT = '!',
  OPERATORS_MOD = '%',
  PUNCTUATORS_LPAREN = '(',
  PUNCTUATORS_RPAREN = ')',
  OPERATORS_MUL = '*',
  OPERATORS_ADD = '+', 
  PUNCTUATORS_COMMA = ',',
  OPERATORS_SUB = '-',
  OPERATORS_DIV = '/',
  PUNCTUATORS_COLON = ':',
  PUNCTUATORS_SEMICOLON = ';',
  OPERATORS_LT = '<',
  OPERATORS_ASSIGN = '=',
  OPERATORS_GT = '>',
  OPERATORS_LE = '<' + double_operator_offset,
  OPERATORS_GE = '>' + double_operator_offset,
  OPERATORS_EQ = '=' + double_operator_offset,
  OPERATORS_NEQ = '!' + double_operator_offset,
  PUNCTUATORS_LBRACKET = '[',
  PUNCTUATORS_RBRACKET = ']',
  PUNCTUATORS_LBRACE = '{',
  PUNCTUATORS_RBRACE = '}',

  KEYWORD_IF = 161,
  KEYWORD_ELSE = 162,
  KEYWORD_TRUE = 163,
  KEYWORD_FALSE = 164,

  LITERALS_INT = 220,
  LITERALS_INT_SUF = LITERALS_INT + suffix_offset,
  LITERALS_FLOAT = 222,
  LITERALS_FLOAT_SUF = LITERALS_FLOAT + suffix_offset,
  
  LITERALS_STRING = 224,
  LITERALS_STRING_VIEW = 225,

  LITERALS_CHARACTER = 226,

  END = 255
};

struct Token {
  TokenClass cls = TokenClass::END;
  TokenType type = TokenType::END;

  std::string str{};
  std::string_view text{};

  std::uint8_t suffix{};

  char32_t character{};

  Location location{};
};

}

#endif