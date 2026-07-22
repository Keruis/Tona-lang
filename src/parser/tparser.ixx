module tona.parser;

import std;

import tona.byte;
import tona.type;
import tona.diag;
import tona.token;
import tona.error;
import tona.bitflag;
import tona.template_utils;

namespace Tona {

  void Parser::parse(std::span<const Token> tokens) {
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

  void Parser::parse_keyword(const Token*& tokens) {
    switch (tokens->type) {
      case TokenType::T_KEYWORD_IF:
      case TokenType::T_KEYWORD_ELSE:
      case TokenType::T_KEYWORD_FOR:
      case TokenType::T_KEYWORD_RETURN:
      case TokenType::T_KEYWORD_TRUE:
      case TokenType::T_KEYWORD_FALSE:
      case TokenType::T_KEYWORD_CONST:
      case TokenType::T_KEYWORD_IMME:
      case TokenType::T_KEYWORD_STATIC:

      default:
        break;
    }
  }

  void Parser::parse_identifier(const Token*& tokens) {

  }

  [[nodiscard]] StorageSpecifiers Parser::parse_storage_specifiers(const Token*& tokens) {
    BitFlag<StorageSpecifiers> bf;
  loop:
    switch (tokens->type) {
      case TokenType::T_KEYWORD_STATIC:
        if (bf.add_if(StorageSpecifiers::SS_STATIC)) [[unlikely]]
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
  
  [[nodiscard]] TypeQualifiers Parser::parse_type_qualifiers(const Token*& tokens) {
    BitFlag<TypeQualifiers> bf;
    TypeQualifiers tq;
  loop:
    switch (tokens->type) {
      case TokenType::T_KEYWORD_CONST:
        tq = TypeQualifiers::TQ_CONST;
        break;
      case TokenType::T_KEYWORD_IMME:
        tq = TypeQualifiers::TQ_IMME;
        break;
      default: goto end; 
    }
    if (bf.add_if(tq)) [[unlikely]]
      diag.push_par_err(
        ErrorLevel::EL_ERROR, 
        ParErrorType::PET_DUPLICATE_TYPE_QUALIFIERS, 
        *tokens
      );
    tokens++;
    goto loop;
  end:
    return bf.value();
  }

  [[nodiscard]] std::string_view Parser::parse_type(const Token*& tokens) {
    if (!any_of_cls<TokenClass::C_IDENTIFIER>(tokens)) [[unlikely]] {
      //
    }
    auto type = tokens->to_str_view();
    tokens++;
    return type;
  } 


  void Parser::parse_var_decl(const Token*& tokens) {
    const auto ss = parse_storage_specifiers(tokens);
    const auto tq = parse_type_qualifiers(tokens);
    if (!any_of_cls<TokenClass::C_IDENTIFIER>(*tokens)) [[unlikely]] {
      //
    }
    std::string_view name = tokens->to_str_view();
    tokens++;
    std::string_view type;
    if (any_of<TokenType::T_PUNCTUATORS_COLON>(tokens))
      type = parse_type(tokens);
    switch(tokens->type) {
      case TokenType::T_PUNCTUATORS_SEMICOLON:
        tokens++;
        break;
      case TokenType::T_OPERATORS_ASSIGN:
        tokens++;
        //
      [[unlikely]] default: {
        //
      }
    }
    //
  }

  template <TokenType... ts>
  [[nodiscard]] bool Parser::any_of(const Token& tok) const noexcept {
    return Utils::any_of<TokenType, ts...>(tok.type);
  }

  template <TokenClass... ts>
  [[nodiscard]] bool Parser::any_of_cls(const Token& tok) const noexcept {
    return Utils::any_of<TokenClass, ts...>(tok.cls);
  }

}