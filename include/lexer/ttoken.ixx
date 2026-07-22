export module tona.token;

import std;

import tona.string;
import tona.byte;

export namespace Tona {

  constexpr std::uint8_t double_char_offset = 93;
  constexpr std::uint8_t suf_offset = 1;

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
    T_KEYWORD_CONST = 167,
    T_KEYWORD_IMME = 168,
    T_KEYWORD_STATIC = 169,

    T_LITERALS_INT = 220,
    T_LITERALS_INT_SUF = 221,
    T_LITERALS_FLOAT = 222,
    T_LITERALS_FLOAT_SUF = 223,
    
    T_LITERALS_STRING = 224,
    _ = 255
  };

  enum class TokenClass : std::uint8_t {
    C_END = 0,
    C_IDENTIFIER,
    C_OPERATOR,
    C_LITERAL,
    C_KEYWORD,
    C_PUNCTUATOR
  };

  [[nodiscard]] constexpr std::uint8_t get_prec(TokenType type) noexcept {
    static constexpr auto prec_table = []{
      std::array<std::uint8_t, 256> table{};
      
      return table;
    }();
    return prec_table[cast_u8(type)];
  }

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
        if (text == "const") return TokenType::T_KEYWORD_CONST;
        break;
      }
      case 6: {
        if (text == "return") return TokenType::T_KEYWORD_RETURN;
        if (text == "static") return TokenType::T_KEYWORD_STATIC;
      }
    }
    return TokenType::T_IDENTIFIER;
  }

  struct alignas(8) Token {
    StringView text;
    const char* start;
    TokenType type;
    TokenClass cls;
    std::uint8_t precedence;
    std::uint8_t suf;

    std::string_view to_str_view() const {
      return std::string_view(text.data, text.len);
    }
  };

  consteval {
    static_assert(sizeof(Token) == 32);
  }

  struct TokenContext {
    std::pmr::vector<std::pmr::vector<Token>> tokens;
    std::vector<std::size_t> path_idx;
  };
}