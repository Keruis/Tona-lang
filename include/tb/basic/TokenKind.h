#include <cstdint>

#ifndef __TOKENKIND__H__
#define __TOKENKIND__H__

namespace tb::token {

enum class TokenClass : std::uint8_t {
  IDENTIFIER,
  KEYWORD,
  OPERATOR,
  LITERAL,
  PUNCTUATOR,
  END
};

}

#endif