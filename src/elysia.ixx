module;

#include "tona.h"

export module tona.elysia;

import tona.lexer;
import tona.token;

export extern "C" {
  
// lexer
  Lexer* create_lexer() {
    auto* lex = new Tona::Lexer();
    return reinterpret_cast<Lexer*>(lex);
  }

  void destroy_lexer(Lexer* lex) {
    delete reinterpret_cast<Tona::Lexer*>(lex);
  }

  void destroy_token_ctx(TokenContext* ctx) {
    delete reinterpret_cast<Tona::TokenContext*>(ctx);
  }

  TokenContext* tokens(Lexer* lex) {
    auto* cpp_lex = reinterpret_cast<Tona::Lexer*>(lex);
    auto* ctx = new Tona::TokenContext(cpp_lex->tokens());
    return reinterpret_cast<TokenContext*>(ctx);
  }

// parser


// sema

}