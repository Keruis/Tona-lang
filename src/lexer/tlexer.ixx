module;

#include <simd>

module tona.lexer;

import std;

import tona.buf;
import tona.byte;
import tona.diag;
import tona.token;
import tona.error;
import tona.arena;
import tona.string;

namespace Tona {

  void Lexer::tokenize(std::string_view text, std::pmr::vector<Token>& tokens, Arena& ca) {
    const char* cur = text.data();
    const char* start_ptr = nullptr;
    TokenType num_type = TokenType::T_LITERALS_INT;
    std::uint64_t val = 0;
    std::uint64_t suf = 0;

    static constexpr void* labels[256] = {
      #include "lexer_label.inc"
    };

    goto *labels[cast_u8(*cur)];
    
    l_skip:
    l_newline:
      goto *labels[cast_u8(*(cur = skip_whitespace(cur)))];
      
    l_identifier:
      goto *labels[cast_u8(parse_identifier(cur, tokens))];

    l_op_chars:
      goto *labels[cast_u8(parse_single_char<TokenClass::C_OPERATOR>(cur, tokens))];

    l_punc_chars:
      goto *labels[cast_u8(parse_single_char<TokenClass::C_PUNCTUATOR>(cur, tokens))];

    l_digit_0:
      start_ptr = cur++;
      switch (*cur) {
        case 'b': case 'B': 
          cur++; 
          goto pn_bin_prefix;
        case 'o': case 'O': 
          cur++; 
          goto pn_oct_prefix;
        case 'x': case 'X': 
          cur++; 
          goto pn_hex_prefix;
        default:
          goto check_numeric_suffix;
      }
      
    l_digit_1_9:
      start_ptr = cur++;
      consume_digit_sequence<
        is_dec_char
      >(cur);
    check_numeric_suffix:
      switch (*cur) {
        case '.': cur++; goto pn_franction_direct;
        case 'e': case 'E': 
          goto pn_exponect_direct;
        case 'u': case 'U':
        case 'i': case 'I': 
          goto pn_suf_num_i;
        case 'f': case 'F':
          num_type = TokenType::T_LITERALS_FLOAT;
          goto pn_suf_num_f;
        [[unlikely]] case '\'': goto pn_error_digit_separator;
        default:
          if (is_identifier_char(*cur)) [[unlikely]]
            goto pn_error_invalid_numeric;
          tokens.push_back({
            .num {
              .val = scan_number<std::uint64_t>(start_ptr, cur),
            },
            .start = start_ptr,
            .type = TokenType::T_LITERALS_INT,
            .cls = TokenClass::C_LITERAL
          });
      }

      goto *labels[cast_u8(*cur)];

    l_string:
      start_ptr = cur;
      if (auto res = read_string(++cur, tokens, ca); res != LexErrorType::LET_NONE) [[unlikely]]
        return make_error(
          ErrorLevel::EL_ERROR,
          res,
          start_ptr, cur
        );

      goto *labels[cast_u8(*cur)];

    l_not:
    l_less:
    l_assign:
    l_greater:
      goto *labels[cast_u8(parse_double_char(cur, tokens))];

    l_div:
      if (!parse_div(cur, tokens)) [[unlikely]]
        return make_error(
          ErrorLevel::EL_ERROR,
          LexErrorType::LET_UNCLOSE_COMMENT, 
          cur, 1
        );

      goto *labels[cast_u8(*cur)];

    pn_bin_prefix:
      if (!parse_radix_digits<is_bin_char, bin_char>(cur)) [[unlikely]]
        goto pn_error_invalid_numeric;
      val = scan_number<std::uint64_t, 2>(start_ptr + 2, cur);
      goto pn_end;
      
    pn_oct_prefix:
      if (!parse_radix_digits<is_oct_char, is_oct_char>(cur)) [[unlikely]]
        goto pn_error_invalid_numeric;
      val = scan_number<std::uint64_t, 8>(start_ptr + 2, cur);
      goto pn_end;

    pn_hex_prefix:
      if (!parse_radix_digits<is_hex_char, is_hex_char>(cur)) [[unlikely]]
        goto pn_error_invalid_numeric;
      val = scan_number<std::uint64_t, 16>(start_ptr + 2, cur);
      goto pn_end;

    pn_franction_direct:
      consume_digit_sequence<
        is_dec_char
      >(cur);

      if (*cur == 'e' || *cur == 'E') {
    pn_exponect_direct:
        cur++;
        if (*cur == '+' || *cur == '-')
          cur++;
        if (!is_dec_char(*cur)) [[unlikely]]
          goto pn_error_invalid_numeric;
        consume_digit_sequence<
          is_dec_char
        >(cur);
      }
      num_type = TokenType::T_LITERALS_FLOAT;
      val = std::bit_cast<std::uint64_t>(scan_number<double>(start_ptr, cur));

    pn_end:
      if (*cur == '\'') [[unlikely]]
        goto pn_error_digit_separator;

      switch (*cur) {
    pn_suf_num_i:
        val = scan_number<std::uint64_t>(start_ptr, cur);
        case 'u': case 'U':
        case 'i': case 'I':
          if (num_type == TokenType::T_LITERALS_FLOAT) [[unlikely]]
            make_error(
              ErrorLevel::EL_WARNING,
              LexErrorType::LET_INVALID_SUFFIX_TYPE,
              start_ptr, cur
            );
        case 'f': case 'F':
    pn_suf_num_f:
          cur++;
          num_type = static_cast<TokenType>(cast_u8(num_type) + suf_offset); 
          goto pn_suf_num;
        default: goto pn_save;
      }

    pn_suf_num:
      while (is_dec_char(*cur))
        suf = suf * 10 + (*cur++ - '0');
    pn_save:
      tokens.push_back({
        .num = {
          .val = val,
          .suf = suf
        },
        .start = start_ptr,
        .type = num_type,
        .cls = TokenClass::C_LITERAL
      });
      suf = 0;
      num_type = TokenType::T_LITERALS_INT;
      goto *labels[cast_u8(*cur)];

    l_end:
      return tokens.push_back({
        .start = cur,
        .type = TokenType::T_END
      });
    
    pn_error_digit_separator:
      return make_error(
        ErrorLevel::EL_WARNING,
        LexErrorType::LET_INVALID_DIGIT_SEPARATOR,
        start_ptr, cur
      );
    
    pn_error_invalid_numeric:
      return make_error(
        ErrorLevel::EL_ERROR,
        LexErrorType::LET_INVALID_NUMERIC_LITERAL,
        start_ptr, cur
      );

    l_default:
      return invalid_char(cur);
  }

  [[nodiscard]] char Lexer::parse_identifier(const char*& cur, std::pmr::vector<Token>& tokens) {
    const std::string_view identifier(cur, identifier_char(cur));
    if (
      auto res = find_keyword(identifier); 
      res == TokenType::T_IDENTIFIER
    ) tokens.push_back({
        .text = {
          .data = cur,
          .len = identifier.size()
        },
        .start = cur,
        .type = TokenType::T_IDENTIFIER,
        .cls = TokenClass::C_IDENTIFIER
      });
    else tokens.push_back({
      .start = cur,
      .type = res,
      .cls = TokenClass::C_KEYWORD
    });
    cur = identifier.cend();
    return *cur;
  }

  [[nodiscard]] bool Lexer::parse_div(const char*& cur, std::pmr::vector<Token>& tokens) {
    switch (*++cur) {
      case '/':
        skip_line_comment(cur);
        return true;
      case '*':
        return skip_block_comment(cur);
      default:
        tokens.push_back({
          .start = cur - 1,
          .type = TokenType::T_OPERATORS_DIV,
          .cls = TokenClass::C_OPERATOR,
          .precedence = get_prec(TokenType::T_OPERATORS_DIV)
        });
        return true;
    }
  } 

  template <TokenClass cls>
  [[nodiscard]] char Lexer::parse_single_char(const char*& cur, std::pmr::vector<Token>& tokens) {
    const TokenType type = static_cast<TokenType>(*cur);
    std::uint8_t prec = 0;
    if constexpr (cls == TokenClass::C_OPERATOR)
      prec = get_prec(type);
    tokens.push_back({
      .start = cur,
      .type = type,
      .cls = cls,
      .precedence = prec
    });
    return *++cur;
  }

  [[nodiscard]] char Lexer::parse_double_char(const char*& cur, std::pmr::vector<Token>& tokens) {
    const std::uint8_t is_double = (cur[1] == '=');
    const TokenType type = static_cast<TokenType>(*cur + is_double * double_char_offset);
    tokens.push_back({
      .start = cur,
      .type = type,
      .cls = TokenClass::C_OPERATOR,
      .precedence = get_prec(type)
    });
    return *(cur += 1 + is_double);
  }

  [[gnu::cold]] void Lexer::invalid_char(const char* cur) {
    std::size_t len = std::countl_one(cast_u8(*cur));
    len = (len == 0 || len > 4) ? 1 : len;
    make_error(
      ErrorLevel::EL_ERROR,
      LexErrorType::LET_INVALID_CHAR,
      cur, len
    );
  }

  template <typename... Args>
  [[gnu::cold]] void Lexer::make_error(ErrorLevel lv, LexErrorType type, Args&&... args) {
    diag.push_lex_err(
      lv, type,
      std::forward<Args>(args)...
    );
  }

  void Lexer::skip_line_comment(const char*& cur) noexcept {
    do {
      cur++;
    } while (*cur != '\n' && *cur != '\0');
  }

  [[nodiscard]] bool Lexer::skip_block_comment(const char*& cur) noexcept {
    while (*cur) {
      if (*cur == '*' && cur[1] == '/') {
        cur += 2;
        return true;
      }
      cur++;
    }
    return false;
  }

  template <auto PredFunc, typename FT>
  void Lexer::consume_digit_sequence(const char*& start) noexcept {
    if constexpr (std::predicate<FT, decltype(*start)>) {
      while (true) {
        if (PredFunc(*start))
          start++;
        else if (*start == '\'' && PredFunc(start[1]))
          start += 2;
        else break;
      }
    } else if constexpr (requires (FT f) {
      start = f(start);
    }) {
      start = PredFunc(start);
    } else static_assert(false, "fun type error");
  }

  template <auto PredFunc1, auto PredFunc2, typename FT1>
  [[nodiscard]] bool Lexer::parse_radix_digits(const char*& start)
    noexcept requires(std::predicate<FT1, decltype(*start)>) 
  {
    if (!PredFunc1(*start)) [[unlikely]]
      return false;
    consume_digit_sequence<
      PredFunc2
    >(start);
    if (!PredFunc1(start[-1])) [[unlikely]]
      return false;
    return true;
  }

  [[nodiscard]] LexErrorType Lexer::read_string(const char*& start, std::pmr::vector<Token>& tokens, Arena& arena) {
    const char* const start_ptr = start;
    const char* prev_ptr = start;

    using Vec = std::simd::vec<std::uint8_t>;

    constexpr Vec ctrl_vec   = std::uint8_t{0x20};
    constexpr Vec quote_vec  = std::uint8_t{'"'};
    constexpr Vec escape_vec = std::uint8_t{'\\'};
    constexpr Vec all_one    = std::uint8_t{1};
    constexpr Vec msb_only   = std::uint8_t{0x80};

    while (true) {
      auto chunk = std::simd::unchecked_load<Vec>(
        reinterpret_cast<const std::uint8_t*>(start), 
        Vec::size()
      );

      auto ctrl_diff   = chunk - ctrl_vec;
      auto quote_diff  = chunk ^ quote_vec;
      auto escape_diff = chunk ^ escape_vec;

      auto quote_minus_one  = quote_diff - all_one;
      auto escape_minus_one = escape_diff - all_one;

      auto final_invalid_mask = (
        (ctrl_diff | quote_minus_one | escape_minus_one) & ~chunk & msb_only
      ) != Vec{};

      if (std::simd::any_of(final_invalid_mask)) [[unlikely]] {
        const char* end_ptr = start + Vec::size();
        const std::size_t offset = std::simd::reduce_min_index(final_invalid_mask);
        start += offset;
        while (start < end_ptr) {
          if (*start == '"') [[likely]] {
            const char* buf_ptr;
            std::size_t len;
            if (!buffer.buf_size()) [[likely]] {
              buf_ptr = start_ptr;
              len = start - start_ptr;
            } else {
              if (start > prev_ptr)
                buffer.stuff_back(prev_ptr, start - prev_ptr);
              buf_ptr = buffer.buffer<const char*>();
              len = buffer.buf_size();
            }
            char* arena_mem = static_cast<char*>(arena.allocate(len, 1));
            std::memcpy(arena_mem, buf_ptr, len);
            tokens.push_back({
              .text = {
                .data = arena_mem,
                .len = len
              },
              .start = start_ptr - 1,
              .type = TokenType::T_LITERALS_STRING,
              .cls = TokenClass::C_LITERAL
            });
            buffer.reset();
            start++;
            return LexErrorType::LET_NONE;
          }

          buffer.stuff_back(prev_ptr, start - prev_ptr);

          switch (*start) {
            case '\\': {
              char cur = 0;
              start += 2;
              switch (start[-1]) {
                case 'n': cur = '\n'; break;
                case 'r': cur = '\r'; break;
                case 't': cur = '\t'; break;
                case 'b': cur = '\b'; break;
                case 'f': cur = '\f'; break;
                case 'v': cur = '\v'; break;
                case 'a': cur = '\a'; break;
                case 'x':
                  for (std::size_t i = 0; i < 2 && is_hex_char(*start); i++, start++) {
                    const char c = *start;
                    cur = cur * 16 + (
                      (c <= '9') ? 
                      (c -  '0') : 
                      ((c | 0x20) - 'a' + 10)
                    );
                  }
                  break;
                default:
                  start--;
                  if (std::size_t i = 2; is_oct_char(*start)) {
                    std::uint64_t val = 0;
                    do {
                      val = val * 8 + (*start++ - '0');
                    } while (i-- && is_oct_char(*start));
                    if (val > 255) [[unlikely]]
                      return LexErrorType::LET_INVALID_ESCAPE_SEQUENCE;
                    cur = val;
                  } else cur = *start++;
              }
              buffer.stuff_back(cur);
              break;
            }
            [[unlikely]] case '\0':
            [[unlikely]] case '\n':
            [[unlikely]] case '\r': return LexErrorType::LET_UNTERMINATED_STRING;
            default:
              buffer.stuff_back(*start++);
          }
          prev_ptr = start;
        }
      } else start += 8;
    }
  }

  template <typename T, std::size_t Base>
  [[nodiscard]] T Lexer::scan_number(const char* start, const char* end) {
    std::size_t n = 0;
    for (; start != end && n < 1023; start++, n++) {
      if (*start == '\'') [[unlikely]]
        continue;
      buffer.stuff_back(cast_u8(*start));
    }
    T val;
    std::from_chars_result res; 
    if constexpr (std::is_floating_point_v<T>)
      res = std::from_chars(buffer.buffer<const char*>(), buffer.buffer<const char*>() + buffer.buf_size(), val);
    else res = std::from_chars(buffer.buffer<const char*>(), buffer.buffer<const char*>() + buffer.buf_size(), val, Base);
    if (res.ec != std::errc{}) [[unlikely]] {
      switch (res.ec) {
        case std::errc::result_out_of_range:
          make_error(ErrorLevel::EL_WARNING, LexErrorType::LET_OUT_OF_RANGE, start - n, end);
          return std::numeric_limits<T>::max();
        case std::errc::invalid_argument:
        default:
          std::unreachable();
      }
    }
    return val;
  }

}