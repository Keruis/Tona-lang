#pragma once

#include <stdint.h>

typedef struct TokenContext TokenContext;
typedef struct Lexer Lexer;

struct TokenError {
  const char* err_cur;
  uint32_t err_len;
};