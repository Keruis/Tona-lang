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
        BitFlag<StorageSpecifiers> bf;
        StorageSpecifiers tq;
      loop:
        switch (tokens->type) {
          case TokenType::T_KEYWORD_STATIC:  
            tq = StorageSpecifiers::SS_STATIC;
      pr:
          if (bf.add_if(tq)) [[unlikely]]
            diag.push_par_err(
              ErrorLevel::EL_ERROR, 
              ParErrorType::PET_DUPLICATE_STORAGE_SPECIFIERS, 
              *tokens
            );
          tokens++;
          goto loop;    
          default: goto end;
        }
      end:
        return bf.value();
      }
      
      [[nodiscard]] TypeQualifiers parse_type_qualifiers(const Token*& tokens) {
        BitFlag<TypeQualifiers> bf;
        TypeQualifiers tq;
      loop:
        switch (tokens->type) {
          case TokenType::T_KEYWORD_CONST: 
            tq = TypeQualifiers::TQ_CONST;
            goto pr;
          case TokenType::T_KEYWORD_IMME:  
            tq = TypeQualifiers::TQ_IMME;
      pr:
          if (bf.add_if(tq)) [[unlikely]]
            diag.push_par_err(
              ErrorLevel::EL_ERROR, 
              ParErrorType::PET_DUPLICATE_TYPE_QUALIFIERS, 
              *tokens
            );
          tokens++;
          goto loop;    
          default: goto end;
        }
      end:
        return bf.value();
      }

    private:
      Diagnostic& diag;
  };

}