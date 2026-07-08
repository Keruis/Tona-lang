export module tona.parser;

import std;

import tona.byte;
import tona.type;
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
        
        static constexpr void* labels[6] = {
          &&p_end, &&p_identifier,
          &&p_operator, &&p_literal,
          &&p_keyword, &&p_punctuator
        };

        goto *labels[cast_u8(cur->cls)];

        p_keyword:
          parse_keyword(cur);
          goto *labels[cast_u8(cur->cls)];

        p_identifier:
          parse_identifier(cur);
          goto *labels[cast_u8(cur->cls)];

        p_operator:

          goto *labels[cast_u8(cur->cls)];

        p_literal:
          goto *labels[cast_u8(cur->cls)];

        
        p_punctuator:
          goto *labels[cast_u8(cur->cls)];

        p_end:

      }

    private:
      void parse_keyword(const Token*& tokens) {

      }

      void parse_identifier(const Token*& tokens) {

      }

    private:
      StorageSpecifiers parse_storage_specifiers(const Token*& tokens) {
        
      }
      
      TypeQualifiers parse_type_qualifiers(const Token*& tokens) {
        // while (tokens->type >= type_qualifier_start && tokens->type <= type_qualifier_end) {
        //   switch (tokens->type) {
        //     case TokenType::T_KEYWORD_CONST:
        //     case TokenType::T_KEYWORD_IMME:
        //   }
        // }
      }

  };

}