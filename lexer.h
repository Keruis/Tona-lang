#include <cctype>
#include <cstdint>
#include <string_view>

namespace T {
  class Lexer {
    public:
      enum class TokenType : std::uint8_t {
        IDENTIFIER = 0,

        OPERATORS_NOT = '!',
        OPERATORS_MOD = '%',
        PUNCTUATORS_LPAREN = '(',
        PUNCTUATORS_RPAREN = ')',
        OPERATORS_MUL = '*',
        OPERATORS_ADD = '+', 
        PUNCTUATORS_COMMA = ',',
        OPERATORS_SUB = '-',
        OPERATORS_DIV = '/',
        PUNCTUATORS_COLON = ':',
        PUNCTUATORS_SEMICOLON = ';',
        OPERATORS_LT = '<',
        OPERATORS_ASSIGN = '=',
        OPERATORS_GT = '>',
        OPERATORS_LE = '<' + 93,
        OPERATORS_GE = '>' + 93,
        OPERATORS_EQ = '=' + 93,
        OPERATORS_NEQ = '!' + 93,
        PUNCTUATORS_LBRACKET = '[',
        PUNCTUATORS_RBRACKET = ']',
        PUNCTUATORS_LBRACE = '{',
        PUNCTUATORS_RBRACE = '}',

        KEYWORD_IF = 161,
        KEYWORD_ELSE = 162,

        LITERALS_INT = 220,
        LITERALS_INT_SUF = 221,
        LITERALS_FLOAT = 222,
        LITERALS_FLOAT_SUF = 223,
        
        LITERALS_STRING = 224
      };

      enum class TokenClass : std::uint8_t {
        IDENTIFIER,
        KEYWORD,
        OPERATOR,
        LITERAL,
        PUNCTUATOR,
        END = 0
      };

    public:
      explicit Lexer(const char* start) noexcept 
        : m_state{.cur = start} {}
      ~Lexer() = default;

      const auto& location() const noexcept {
        return m_state.location;
      }

      void next_token() noexcept {
        auto& start = m_state.start;
        auto& cur = m_state.cur;
        auto& line = m_state.location.line;
        auto& column = m_state.location.column;
        auto& token = m_state.token;
        auto& text = m_state.token.text;

        static void* labels[] = {
#pragma region labels
          &&L_end, &&L_default, &&L_default, &&L_default, 
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_skip, &&L_newline, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_skip, &&L_not, &&L_string, &&L_default,
          &&L_default, &&L_op, &&L_default, &&L_default,
          &&L_punc, &&L_punc, &&L_op, &&L_op,
          &&L_punc, &&L_op, &&L_default, &&L_div,
          &&L_digit_0, &&L_digit_1_9, &&L_digit_1_9, &&L_digit_1_9,
          &&L_digit_1_9, &&L_digit_1_9, &&L_digit_1_9, &&L_digit_1_9,
          &&L_digit_1_9, &&L_digit_1_9, &&L_punc, &&L_punc,
          &&L_less, &&L_assign, &&L_greater, &&L_default, &&L_default,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_punc, &&L_default,
          &&L_punc, &&L_default, &&L_identifier, &&L_default,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_identifier, &&L_identifier,
          &&L_identifier, &&L_identifier, &&L_punc, &&L_default,
          &&L_punc, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default, &&L_default,
          &&L_default, &&L_default, &&L_default,
#pragma endregion
        };

        goto *labels[*cur];

      L_newline:
          line++;
      L_skip:
          column++;
          goto *labels[*++cur];

      L_identifier:
          start = cur;
          do
            cur++;
          while (std::isalpha(
            static_cast<int>(static_cast<unsigned char>(*cur))
          ) || *cur == '_');
          text = {start, static_cast<std::size_t>(cur - start)};
          token.type = find_keyword(text);
          token.cls = static_cast<TokenClass>(static_cast<bool>(token.type));
          column += text.size();
          return;

      L_op:
          token.cls = TokenClass::OPERATOR;
          goto set_class;

      L_punc:
          token.cls = TokenClass::PUNCTUATOR;
        set_class:
          start = cur;
          token.type = static_cast<TokenType>(*cur++);
          column++;
          return;

      L_digit_0:
          token.type = TokenType::LITERALS_INT;
          start = cur++;
          switch (*cur) {
            case 'b': case 'B':
              cur++;
              goto bin_prefix;
            case 'o': case 'O':
              cur++;
              goto oct_prefix;
            case 'x': case 'X':
              cur++;
              goto hex_prefix;
            default:
              goto scan_numeric_suffix;
          }

      L_digit_1_9:
          token.type = TokenType::LITERALS_INT;
          start = cur++;
          scan_digit_sequence<
            std::isdigit
          >(cur);
        scan_numeric_suffix:
          switch (*cur++) {
            case '.': 
            case 'e': case 'E':
            case 'u': case 'U':
            case 'i': case 'I':
              goto suf_num_i;
            case 'f': case 'F':
              goto suf_num_f;
            default:
              if (std::isalnum(static_cast<int>(static_cast<unsigned char>(*cur))) || *cur == '_') [[unlikely]]
                ;
              text = {start, static_cast<std::size_t>(cur - start)};
              token.type = TokenType::LITERALS_INT;
              token.cls = TokenClass::LITERAL;
              column += text.size();
              return;
          }
        
      L_string:

      L_not:
      L_less:
      L_assign:
      L_greater:
          token.is_double = cur[1] == '=';
          token.type = static_cast<TokenType>(*cur + token.is_double * 93);
          token.cls = TokenClass::OPERATOR;
          cur += (1 + token.is_double);
          return;

      L_div:

        bin_prefix:
          scan_digit_sequence<
            [](int c) {
              return c == '0' || c == '1';
            }
          >(cur);
          goto numeric_end;  
        oct_prefix:
          scan_digit_sequence<
            [](int c){
              return c >= '0' && c <= '7';
            }
          >(cur);
          goto numeric_end; 
        hex_prefix:
          scan_digit_sequence<
            [](int c) {
              return std::isdigit(c) || c >= 'A' && c <= 'F';
            }
          >(cur);
          goto numeric_end; 

        franction_direct:
          scan_digit_sequence<
            std::isdigit
          >(cur);
          if (*cur == 'e' || *cur == 'E') { 
        exponect_direct:
            cur++;
            cur += (*cur == '+' || *cur == '-');
            if (!std::isdigit(*cur)) [[unlikely]]
              ;
            scan_digit_sequence<
              std::isdigit
            >(cur);
          }
          token.type = TokenType::LITERALS_FLOAT;

        numeric_end:
          switch (*cur) {
        suf_num_i:
            case 'i': case 'I':
            case 'u': case 'U':
              if (token.type == TokenType::LITERALS_FLOAT)
                ;
              token.type = TokenType::LITERALS_INT;
        suf_num_f:
            case 'f': case 'F':
              cur++;
              token.type = static_cast<TokenType>(static_cast<std::uint8_t>(token.type) + 1);
              token.suf_num = 0;
              goto suf_num;
            default:
              goto save;
          }
        suf_num:
          while (std::isdigit(static_cast<int>(static_cast<unsigned char>(*cur))))
            token.suf_num = token.suf_num * 10 + (*cur++ - '0');
          if (token.suf_num != 8 || token.suf_num != 16 || token.suf_num != 32 || token.suf_num != 64)
            ;
        save:
          text = {start, static_cast<std::size_t>(cur - start) - (token.suf_num >= 10 ? 2 : 1)};
          return;

      L_end:
          token.cls = TokenClass::END;
          return;

      L_default: 
        {
          return;
        }
      }

    private:
      TokenType find_keyword(std::string_view text) const noexcept {
        using enum TokenType;
        if (text == "if")
          return KEYWORD_IF;
        if (text == "else")
          return KEYWORD_ELSE;
        return IDENTIFIER;
      }

      template <auto F>
      void scan_digit_sequence(const char*& begin) const noexcept {
        while (F(static_cast<int>(static_cast<unsigned char>(*begin))))
          begin++;
      }

      bool is_bin_digit(int c) const noexcept {
        return c == '0' || c == '1';
      }

    private:
      struct {
        const char* start;
        const char* cur;
        struct {
          std::size_t line;
          std::size_t column;
        } location;
        struct {
          std::string_view text;
          TokenClass cls;
          TokenType type;
          union {
            std::uint8_t suf_num;
            std::uint8_t is_double;
          };
        } token;
      } m_state;
  };
}