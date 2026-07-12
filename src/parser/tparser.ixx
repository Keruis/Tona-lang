module;
#include <initializer_list>
export module tona.parser;

import std;

import tona.byte;
import tona.type;
import tona.diag;
import tona.token;
import tona.error;
import tona.bitflag;

export namespace Tona {

  class Parser {
    public:
      Parser(Diagnostic& diag) 
        : diag(diag) 
      {}
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
      [[nodiscard]] StorageSpecifiers parse_storage_specifiers(const Token*& tokens) {
        return parse_flags<StorageSpecifiers, ParErrorType::PET_DUPLICATE_STORAGE_SPECIFIERS>(
          tokens, 
          {
            {TokenType::T_KEYWORD_STATIC, StorageSpecifiers::SS_STATIC}
          }
        );
      }
      
      [[nodiscard]] TypeQualifiers parse_type_qualifiers(const Token*& tokens) {
        return parse_flags<TypeQualifiers, ParErrorType::PET_DUPLICATE_TYPE_QUALIFIERS>(
          tokens, 
          {
            {TokenType::T_KEYWORD_CONST, TypeQualifiers::TQ_CONST},
            {TokenType::T_KEYWORD_IMME, TypeQualifiers::TQ_IMME}
          }
        );
      }

    private:
      template <typename E, ParErrorType ER>
      [[nodiscard]] [[gnu::always_inline]] inline E parse_flags(
        const Token*& tokens, 
        std::initializer_list<std::pair<TokenType, E>> table
      ) {
        BitFlag<E> bf;
      loop:
        for (auto [type, flag] : table) {
          if (tokens->type != type)
            continue;
          if (bf.add_if(flag)) [[unlikely]]
            diag.push_par_err(
              ErrorLevel::EL_ERROR, 
              ER, 
              *tokens
            );
          ++tokens;
          goto loop;
        }
        return bf.value();
      }

    private:
      Diagnostic& diag;
  };

}