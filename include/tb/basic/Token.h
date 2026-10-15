#ifndef __TOKEN__H__
#define __TOKEN__H__

#include "TokenKind.h"
#include "Location.h"

#include <string>
#include <string_view>

namespace tb {

constexpr static std::uint8_t repeated_char_operator_offset = 90;
constexpr static std::uint8_t equals_suffix_operator_offset = 95;
constexpr static std::uint8_t literal_suffix_offset = 1;

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
  
  PUNCTUATORS_LBRACKET = '[',
  PUNCTUATORS_RBRACKET = ']',
  PUNCTUATORS_LBRACE = '{',
  PUNCTUATORS_RBRACE = '}',

  OPERATORS_NEQ = '!' + equals_suffix_operator_offset,
  
  OPERATORS_INC = '+' + repeated_char_operator_offset,
  OPERATORS_DEC = '-' + repeated_char_operator_offset,

  OPERATORS_PE = '+' + equals_suffix_operator_offset,
  OPERATORS_RE = '%' + equals_suffix_operator_offset,
  OPERATORS_ME = '*' + equals_suffix_operator_offset,
  OPERATORS_SE = '-' + equals_suffix_operator_offset,
  OPERATORS_DE = '/' + equals_suffix_operator_offset,

  OPERATORS_LSHIFT = '<' + repeated_char_operator_offset,
  OPERATORS_RSHIFT = '>' + repeated_char_operator_offset,

  OPERATORS_LE = '<' + equals_suffix_operator_offset,
  OPERATORS_EQ = '=' + equals_suffix_operator_offset,
  OPERATORS_GE = '>' + equals_suffix_operator_offset,

  KEYWORD_IF = 161,
  KEYWORD_ELSE = 162,
  KEYWORD_TRUE = 163,
  KEYWORD_FALSE = 164,

  LITERALS_INT = 220,
  LITERALS_INT_SUF = LITERALS_INT + literal_suffix_offset,
  LITERALS_FLOAT = 222,
  LITERALS_FLOAT_SUF = LITERALS_FLOAT + literal_suffix_offset,
  
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