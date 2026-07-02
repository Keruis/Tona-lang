export module tona.parser;

import std;

import tona.byte;
import tona.token;

export namespace Tona {

  class Parser {
    public:
      Parser(const Parser&) = delete;
      Parser& operator=(const Parser&) = delete;
      Parser(Parser&&) = delete;
      Parser& operator=(Parser&&) = delete;

      void parse(std::span<const Token> tokens) {
        const Token* cur = tokens.data();

        switch (tokens_info[cast_u8(cur->type)].cls) {
          case TokenClass::C_IDENTIFIER:
          case TokenClass::C_KEYWORD:
          case TokenClass::C_LITERAL:
          case TokenClass::C_OPERATOR:
          case TokenClass::C_PUNCTUATOR:
          case TokenClass::C_END:
        }
      }

    private:


  };

}