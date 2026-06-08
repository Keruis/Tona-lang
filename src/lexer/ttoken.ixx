export module tona.token;

import std;

import tona.types;

export namespace Tona {

  enum class TokenType : u8 {
    T_END = 0,

    T_OPERATORS_NOT = '!', // 33
    T_OPERATORS_MOD = '%',  // 37
    T_PUNCTUATORS_LPAREN = '(', // 40
    T_PUNCTUATORS_RPAREN = ')', // 41
    T_OPERATORS_MUL = '*',  // 42
    T_OPERATORS_ADD = '+',  // 43
    T_PUNCTUATORS_COMMA = ',',  // 44
    T_OPERATORS_SUB = '-',  // 45
    T_OPERATORS_DIV = '/',  // 47
    T_PUNCTUATORS_COLON = ':',  // 58
    T_PUNCTUATORS_SEMICOLON = ';',  // 59
    T_OPERATORS_LT = '<', // 60
    T_OPERATORS_ASSIGN = '=', // 61
    T_OPERATORS_GT = '>', // 62
    T_OPERATORS_LE = '>' + 1, // 63
    T_OPERATORS_GE = '>' + 2, // 64
    T_OPERATORS_EQ = '>' + 3, // 65
    T_OPERATORS_NEQ = '>' + 4,  // 66
    T_PUNCTUATORS_LBRACKET = '[', // 91
    T_PUNCTUATORS_RBRACKET = ']', // 93
    T_PUNCTUATORS_LBRACE = '{', // 123
    T_PUNCTUATORS_RBRACE = '}', // 125

    T_IDENTIFIER = 160,

    T_KEYWORD_VAR = 161,
    T_KEYWORD_IF = 162,
    T_KEYWORD_ELSE = 163,
    T_KEYWORD_FUN = 164,
    T_KEYWORD_FOR = 165, //166,
    T_KEYWORD_TRUE = 166, //167,
    T_KEYWORD_FALSE = 167, //168,

    T_LITERALS_INT = 220,
    T_LITERALS_FLOAT = 221,
    T_LITERALS_INT_SUF = 222,
    T_LITERALS_FLOAT_SUF = 223,
    T_LITERALS_STRING = 224,
    _ = 255
  };

  enum class TokenClass : std::uint32_t {
    C_END = 0,
    C_IDENTIFIER,
    C_OPERATOR,
    C_LITERAL,
    C_KEYWORD,
    C_PUNCTUATOR
  };

  struct TokenInfo {
    TokenClass cls = TokenClass::C_END;
    std::uint32_t precedence = 0;
  };

  constexpr auto tokens_info = [] {
    std::array<TokenInfo, 256> table{};

    auto op = [&](TokenType t, std::uint32_t prec) { 
      table[cast_usize(t)] = {TokenClass::C_OPERATOR, prec}; 
    };
    
    auto pun = [&](TokenType t) { 
      table[cast_usize(t)] = {TokenClass::C_PUNCTUATOR, 0}; 
    };

    auto kw = [&](TokenType t) { 
      table[cast_usize(t)] = {TokenClass::C_KEYWORD, 0}; 
    };

    op(TokenType::T_OPERATORS_ASSIGN, 1);
    op(TokenType::T_OPERATORS_EQ, 2);
    op(TokenType::T_OPERATORS_LT, 2);
    op(TokenType::T_OPERATORS_GT, 2);
    op(TokenType::T_OPERATORS_LE, 2);
    op(TokenType::T_OPERATORS_GE, 2);
    op(TokenType::T_OPERATORS_NEQ, 2);
    op(TokenType::T_OPERATORS_ADD, 3);
    op(TokenType::T_OPERATORS_SUB, 3);
    op(TokenType::T_OPERATORS_MUL, 4);
    op(TokenType::T_OPERATORS_DIV, 4);
    op(TokenType::T_OPERATORS_MOD, 4);

    pun(TokenType::T_PUNCTUATORS_LPAREN);
    pun(TokenType::T_PUNCTUATORS_RPAREN);
    pun(TokenType::T_PUNCTUATORS_LBRACKET);
    pun(TokenType::T_PUNCTUATORS_RBRACKET);
    pun(TokenType::T_PUNCTUATORS_LBRACE);
    pun(TokenType::T_PUNCTUATORS_RBRACE);
    pun(TokenType::T_PUNCTUATORS_COMMA);
    pun(TokenType::T_PUNCTUATORS_COLON);
    pun(TokenType::T_PUNCTUATORS_SEMICOLON);

    kw(TokenType::T_KEYWORD_VAR);
    kw(TokenType::T_KEYWORD_IF);
    kw(TokenType::T_KEYWORD_ELSE);
    kw(TokenType::T_KEYWORD_FUN);
    kw(TokenType::T_KEYWORD_FOR);
    kw(TokenType::T_KEYWORD_TRUE);
    kw(TokenType::T_KEYWORD_FALSE);

    table[cast_usize(TokenType::T_LITERALS_INT)]    = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_FLOAT)]  = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_INT_SUF)]    = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_FLOAT_SUF)]  = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_STRING)] = {TokenClass::C_LITERAL, 0};

    table[cast_usize(TokenType::T_IDENTIFIER)] = {TokenClass::C_IDENTIFIER, 0};

    table[cast_usize(TokenType::T_END)] = {TokenClass::C_END, 0};

    return table;
  }();

  constexpr usize keyword_start_index = cast_usize(TokenType::T_KEYWORD_VAR);

  struct Keyword {
    std::string_view key;
    TokenType type;
  };

  const std::flat_map<std::string_view, TokenType> keywords {
    {"var", TokenType::T_KEYWORD_VAR},
    {"if", TokenType::T_KEYWORD_IF},
    {"else", TokenType::T_KEYWORD_ELSE},
    {"fn", TokenType::T_KEYWORD_FUN},
    {"for", TokenType::T_KEYWORD_FOR},
    {"true", TokenType::T_KEYWORD_TRUE},
    {"false", TokenType::T_KEYWORD_FALSE}
  };

  TokenType find_keyword(std::string_view text) {
    if (
      auto it = keywords.find(text); 
      it != keywords.end()
    ) return it->second;
    return TokenType::T_IDENTIFIER;
  }

  struct alignas(8) Token {
    union {
      std::string_view text;
      usize str_idx;
    };
    t_rcp start;
    TokenType type;
  };

  struct TokenContext {
    std::vector<Token> tokens;
    std::vector<std::string> strings;
    usize path_idx;
  };
}