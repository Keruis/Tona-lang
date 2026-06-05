module;

#include "tona.h"

export module tona.elysia;

import tona.types;
import tona.lexer;
import tona.token;

import std;

export extern "C" {
  Tona::Lexer* create_lexer() {
    return new Tona::Lexer();
  }

  void destroy_lexer(Tona::Lexer* lex) {
    delete lex;
  }

  void destroy_token_ctx(Tona::TokenContext* ctx) {
    delete ctx;
  }

  Tona::TokenContext* tokens(
    Tona::Lexer* lex, Tona::tccp path_ptr, std::size_t path_len, 
    Tona::tccp text_ptr, std::size_t text_len, TokenError* err
  ) {
    if (auto res = lex->tokens(
      std::string_view(path_ptr, path_len), 
      std::string_view(text_ptr, text_len)); 
      res.has_value()
    ) return new Tona::TokenContext(std::move(*res));
    else {
      if (err) *err = res.error();
    } return nullptr;
  }

}



