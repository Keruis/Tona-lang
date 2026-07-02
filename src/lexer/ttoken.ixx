export module tona.token;

import std;

import tona.byte;

export namespace Tona {

  constexpr std::uint8_t double_char_offset = 93;

  enum class TokenType : std::uint8_t {
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
    T_OPERATORS_LE = '<' + double_char_offset, // 147 <=
    T_OPERATORS_GE = '>' + double_char_offset, // 149 >=
    T_OPERATORS_EQ = '=' + double_char_offset, // 148 ==
    T_OPERATORS_NEQ = '!' + double_char_offset,  // 120 !=
    T_PUNCTUATORS_LBRACKET = '[', // 91
    T_PUNCTUATORS_RBRACKET = ']', // 93
    T_PUNCTUATORS_LBRACE = '{', // 123
    T_PUNCTUATORS_RBRACE = '}', // 125

    T_IDENTIFIER = 160,

    T_KEYWORD_IF = 161,
    T_KEYWORD_ELSE = 162,
    T_KEYWORD_FOR = 163,
    T_KEYWORD_TRUE = 164,
    T_KEYWORD_FALSE = 165,
    T_KEYWORD_RETURN = 166,

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
    op(TokenType::T_OPERATORS_NOT, 0);

    pun(TokenType::T_PUNCTUATORS_LPAREN);
    pun(TokenType::T_PUNCTUATORS_RPAREN);
    pun(TokenType::T_PUNCTUATORS_LBRACKET);
    pun(TokenType::T_PUNCTUATORS_RBRACKET);
    pun(TokenType::T_PUNCTUATORS_LBRACE);
    pun(TokenType::T_PUNCTUATORS_RBRACE);
    pun(TokenType::T_PUNCTUATORS_COMMA);
    pun(TokenType::T_PUNCTUATORS_COLON);
    pun(TokenType::T_PUNCTUATORS_SEMICOLON);

    kw(TokenType::T_KEYWORD_IF);
    kw(TokenType::T_KEYWORD_ELSE);
    kw(TokenType::T_KEYWORD_FOR);
    kw(TokenType::T_KEYWORD_TRUE);
    kw(TokenType::T_KEYWORD_FALSE);
    kw(TokenType::T_KEYWORD_RETURN);

    table[cast_usize(TokenType::T_LITERALS_INT)]    = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_FLOAT)]  = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_INT_SUF)]    = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_FLOAT_SUF)]  = {TokenClass::C_LITERAL, 0};
    table[cast_usize(TokenType::T_LITERALS_STRING)] = {TokenClass::C_LITERAL, 0};

    table[cast_usize(TokenType::T_IDENTIFIER)] = {TokenClass::C_IDENTIFIER, 0};

    table[cast_usize(TokenType::T_END)] = {TokenClass::C_END, 0};

    return table;
  }();

  [[nodiscard]] constexpr TokenType find_keyword(std::string_view text) noexcept {
    switch (text.size()) {
      case 2: {
        if (text == "if") return TokenType::T_KEYWORD_IF;
        break;
      }
      case 3: {
        if (text == "for") return TokenType::T_KEYWORD_FOR;
        break;
      }
      case 4: {
        if (text == "else") return TokenType::T_KEYWORD_ELSE;
        if (text == "true") return TokenType::T_KEYWORD_TRUE;
        break;
      }
      case 5: {
        if (text == "false") return TokenType::T_KEYWORD_FALSE;
        break;
      }
      case 6: {
        if (text == "return") return TokenType::T_KEYWORD_RETURN;
      }
    }
    return TokenType::T_IDENTIFIER;
  }

  struct alignas(8) Token {
    struct {
      const char* data;
      std::size_t len;
    } text;
    const char* start;
    TokenType type;
  };

  struct TokenContext {
    std::pmr::vector<std::pmr::vector<Token>> tokens;
    std::vector<std::size_t> path_idx;
  };
}