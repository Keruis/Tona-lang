export module tona.lexer;

import tona.token;

import std;

export namespace Tona {

  class Lexer {
    public:
    [[nodiscard]] TokenContext tokens() {
      TokenContext ctx;
      ctx.path = "123";

      return ctx;
    }
  };

}