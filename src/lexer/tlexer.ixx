export module tona.lexer;

import std;

import tona.token;
import tona.buf;
import tona.byte;
import tona.error;
import tona.arena;

export namespace Tona {

  class Lexer {
    public:
      [[nodiscard]] std::expected<TokenContext, LexError> tokenize(std::string_view text, Arena& arena) noexcept {
        TokenContext ctx {
          .tokens = std::pmr::vector<Token>(&arena)
        };

        const char* cur = text.data();
        const char* start_ptr = nullptr;
        TokenType num_type = TokenType::T_LITERALS_INT;

        static constexpr void* labels[256] = {
          #include "lexer_label.inc"
        };

        goto *labels[cast_u8(*cur)];
        
        l_skip: // ' '
        l_newline: // '\n'
          goto *labels[cast_u8(*(cur = skip_whitespace(cur)))];
          
        l_identifier: // a - z A - Z _
          goto *labels[cast_u8(*(cur = parse_identifier(cur, ctx)))];

        l_op_chars: // ! % * + - /
        l_punc_chars: // () [] : ; {}
          goto *labels[cast_u8(*(cur = parse_single_char(cur, ctx)))];

        l_digit_0: // 0
          start_ptr = cur++;
          switch (*cur) {
            case 'b' :
            case 'B' : cur++; goto pn_bin_prefix;
            case 'o' :
            case 'O' : cur++; goto pn_oct_prefix;
            case 'x' :
            case 'X' : cur++; goto pn_hex_prefix;
            default:
              goto check_numeric_suffix;
          }
          
        l_digit_1_9: // 1 - 9
          start_ptr = cur++;
          cur = consume_digit_sequence<
            is_dec_char
          >(cur);
        check_numeric_suffix:
          switch (*cur) {
            case '.' : cur++; goto pn_franction_direct;
            case 'e' :
            case 'E' : cur++; goto pn_exponect_direct;
            case 'u' :
            case 'U' :
            case 'i' :
            case 'I' : cur++; goto pn_suf_num_i;
            case 'f' :
            case 'F' : cur++; goto pn_suf_num_f;
            [[unlikely]] case '\'': return std::unexpected(
              LexError{
                .err_text = std::string_view(start_ptr, cur),
                .type = LexErrorType::LET_INVALID_DIGIT_SEPARATOR
              }
            );
            default:
              if (is_identifier_char(*cur)) [[unlikely]]
                goto pn_error;
              ctx.tokens.push_back({
                .text = {
                  .data = start_ptr, 
                  .len = cast_usize(cur - start_ptr)
                },
                .start = start_ptr,
                .type = TokenType::T_LITERALS_INT
              });
          }

          goto *labels[cast_u8(*cur)];

        l_string: // "
          if (auto res = read_string(&cur[1], ctx, arena); !res.has_value()) [[unlikely]]
            return std::unexpected(
              LexError{
                .err_text = std::string_view(cur, res.error()),
                .type = LexErrorType::LET_UNTERMINATED_STRING
              }
            );
          else cur = res.value();
          goto *labels[cast_u8(*cur)];

        { // = == < <= > >= ! !=
          TokenType double_type;
        l_assign:
          double_type = TokenType::T_OPERATORS_EQ;
          goto check_double_type;
        l_less:
          double_type = TokenType::T_OPERATORS_LE;
          goto check_double_type;
        l_greater:
          double_type = TokenType::T_OPERATORS_GE;
          goto check_double_type;
        l_not:
          double_type = TokenType::T_OPERATORS_NEQ;
        check_double_type:
          goto *labels[cast_u8(*(cur = parse_double_char(cur, double_type, ctx)))];
        }

        l_div: // / // /* */
          if (auto res = parse_div(cur, ctx)) [[likely]]
            goto *labels[cast_u8(*(cur = res.value()))];
          else
            return std::unexpected(res.error());

        pn_bin_prefix:
          cur = consume_digit_sequence<
            bin_char
          >(cur);

          goto pn_end;
          
        pn_oct_prefix:
          cur = consume_digit_sequence<
            is_oct_char
          >(cur);

          goto pn_end;

        pn_hex_prefix:
          cur = consume_digit_sequence<
            is_hex_char
          >(cur);

          goto pn_end;

        pn_franction_direct:
          cur = consume_digit_sequence<
            is_dec_char
          >(cur);

          if (*cur == 'e' || *cur == 'E') {
        pn_exponect_direct:
            if (*cur == '+' || *cur == '-')
              cur++;
            if (!is_dec_char(*cur)) [[unlikely]]
              goto pn_error;
            cur = consume_digit_sequence<
              is_dec_char
            >(cur);
          }
          num_type = TokenType::T_LITERALS_FLOAT;

        pn_end:
          if (cur[-1] == '\'') [[unlikely]]
            goto pn_error;
  
          switch (*cur) {
            case 'u':
            case 'U':
            case 'i':
            case 'I':
        pn_suf_num_i: 
              num_type = TokenType::T_LITERALS_INT_SUF; 
              break;
            case 'f':
            case 'F':
        pn_suf_num_f: 
              num_type = TokenType::T_LITERALS_FLOAT_SUF; 
              break;
            default: goto pn_save;
          }
        pn_suf_num:
          while (is_dec_char(*cur))
            cur++;
        pn_save:
          ctx.tokens.push_back({
            .text = {
              .data = start_ptr, 
              .len = cast_usize(cur - start_ptr)
            },
            .start = start_ptr,
            .type = num_type
          });

          num_type = TokenType::T_LITERALS_INT;
          goto *labels[cast_u8(*cur)];

        pn_error:
          return std::unexpected(
            LexError{
              .err_text = std::string_view(start_ptr, cur),
              .type = LexErrorType::LET_INVALID_NUMERIC_LITERAL
            }
          );

        l_end:
          ctx.tokens.push_back({
            .start = cur,
            .type = TokenType::T_END
          });

          return ctx;

        l_default:
          return std::unexpected(parse_invalid_char(cur));
      }

    private:
      [[nodiscard]] [[gnu::always_inline]] inline const char* skip_whitespace(const char* cur) noexcept {
        do {
          cur++;
        } while (*cur == ' ' || *cur == '\t' || *cur == '\n');
        return cur;
      }

      [[nodiscard]] [[gnu::always_inline]] inline const char* parse_identifier(const char* cur, TokenContext& ctx) noexcept {
        const char* const end_ptr = identifier_char(cur);
        std::string_view identifier(cur, end_ptr);
        if (
          auto res = find_keyword(identifier); 
          res == TokenType::T_IDENTIFIER
        ) ctx.tokens.push_back({
            .text = {
              .data = cur,
              .len = identifier.size()
            },
            .start = cur,
            .type = TokenType::T_IDENTIFIER
          });
        else ctx.tokens.push_back({
          .start = cur,
          .type = res
        });
        return end_ptr;
      }

      [[nodiscard]] [[gnu::always_inline]] inline std::expected<const char*, LexError> parse_div(const char* cur, TokenContext& ctx) noexcept {
        if (*++cur == '/') {
          do {
            cur++;
          } while (*cur != '\n' && *cur != '\0');
        } else if (*cur == '*') {
          while (true) {
            cur++;
            if (*cur == '\0') [[unlikely]]
              return std::unexpected(
                LexError{
                  .err_text = std::string_view(cur, 1),
                  .type = LexErrorType::LET_UNCLOSE_COMMENT
                }
              );
            if (*cur == '*' && cur[1] == '/')
              break;
          }
          cur += 2;
        } else ctx.tokens.push_back({
            .start = cur - 1,
            .type = TokenType::T_OPERATORS_DIV
          });
        return cur;
      } 

      [[nodiscard]] [[gnu::always_inline]] inline const char* parse_single_char(const char* cur, TokenContext& ctx) noexcept {
        ctx.tokens.push_back({
          .start = cur,
          .type = static_cast<TokenType>(*cur)
        });
        return cur + 1;
      }

      [[nodiscard]] [[gnu::always_inline]] inline const char* parse_double_char(const char* cur, TokenType double_type, TokenContext& ctx) noexcept {
        const char* const start_ptr = cur;
        if (cur[1] == '=') {
          cur += 2;
          ctx.tokens.push_back({
            .start = start_ptr,
            .type = double_type
          });
        } else {
          cur++;
          ctx.tokens.push_back({
            .start = start_ptr,
            .type = static_cast<TokenType>(
              static_cast<std::uint8_t>(double_type) - double_char_offset
            )
          });
        }
        return cur;
      }

      [[nodiscard]] LexError parse_invalid_char(const char* cur) noexcept {
        std::size_t len = std::countl_one(static_cast<std::uint8_t>(*cur));
        len = (len == 0 || len > 4) ? 1 : len;
        return {
          .err_text = std::string_view(cur, len),
          .type = LexErrorType::LET_INVALID_CHAR
        };
      }

    private:
      template <auto PredFunc, typename FT = decltype(PredFunc)>
      [[nodiscard]] [[gnu::always_inline]] inline const char* consume_digit_sequence(const char* start) noexcept {
        if constexpr (std::predicate<FT, decltype(*start)>) {
          while (true) {
            if (PredFunc(*start))
              start++;
            else if (*start == '\'' && PredFunc(start[1]))
              start+=2;
            else break;
          }
        } else if constexpr (requires (FT f) {
          start = f(start);
        }) {
          start = PredFunc(start);
        } else static_assert(false, "fun type error");
        return start;
      }

      [[nodiscard]] [[gnu::always_inline]] inline std::expected<const char*, const char*> read_string(const char* start, TokenContext& ctx, Arena& arena) noexcept {
        const char* const start_ptr = start - 1;
        const char* prev = start;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8);

          const std::uint64_t quote_match        = chunk_bytes ^ SWAR64::char_mask<'"'>;
          const std::uint64_t escape_match       = chunk_bytes ^ SWAR64::char_mask<'\\'>;
          const std::uint64_t carriage_match     = chunk_bytes ^ SWAR64::char_mask<'\r'>;
          const std::uint64_t newline_match      = chunk_bytes ^ SWAR64::char_mask<'\n'>;

          const std::uint64_t null_minus_one     = chunk_bytes - SWAR64::all_bytes_one;
          const std::uint64_t quote_minus_one    = quote_match - SWAR64::all_bytes_one;
          const std::uint64_t escape_minus_one   = escape_match - SWAR64::all_bytes_one;
          const std::uint64_t carriage_minus_one = carriage_match - SWAR64::all_bytes_one;
          const std::uint64_t newline_minus_one  = newline_match - SWAR64::all_bytes_one;

          const std::uint64_t null_inverse       = ~chunk_bytes;
          const std::uint64_t quote_inverse      = ~quote_match;
          const std::uint64_t escape_inverse     = ~escape_match;
          const std::uint64_t carriage_inverse   = ~carriage_match;
          const std::uint64_t newline_inverse    = ~newline_match;

          const std::uint64_t null_msb_match     = null_minus_one & null_inverse;
          const std::uint64_t quote_msb_match    = quote_minus_one & quote_inverse;
          const std::uint64_t escape_msb_match   = escape_minus_one & escape_inverse;
          const std::uint64_t carriage_msb_match = carriage_minus_one & carriage_inverse;
          const std::uint64_t newline_msb_match  = newline_minus_one & newline_inverse;

          const std::uint64_t final_invalid_flags = (null_msb_match | quote_msb_match | escape_msb_match | carriage_msb_match | newline_msb_match) & SWAR64::msb_only_mask;

          if (final_invalid_flags) [[unlikely]] {
            const std::size_t size = match_offset(final_invalid_flags);
            start += size;
            buffer.stuff_back(prev, start - prev);
            const char* const end = start + 8 - size;
            while (start < end) {
              switch (*start) {
                case '"' : {
                  char* arena_mem = static_cast<char*>(arena.allocate(buffer.buf_size(), 1));
                  std::memcpy(arena_mem, buffer.buffer<const char*>(), buffer.buf_size());
                  ctx.tokens.push_back({
                    .text = {
                      .data = arena_mem,
                      .len = buffer.buf_size()
                    },
                    .start = start_ptr,
                    .type = TokenType::T_LITERALS_STRING
                  });
                  buffer.reset();
                  return start + 1;
                }
                case '\0':
                case '\n':
                case '\r': return std::unexpected(start);
                case '\\': {
                  const char* escape_ptr = start;
                  std::uint8_t cur = 0;
                  switch (*++start) {
                    case 'n': cur = '\n'; break;
                    case 'r': cur = '\r'; break;
                    case 't': cur = '\t'; break;
                    case 'b': cur = '\b'; break;
                    case 'f': cur = '\f'; break;
                    case 'v': cur = '\v'; break;
                    case 'a': cur = '\a'; break;
                    case 'x': start++;
                      for (std::size_t i = 0; i < 2 && is_hex_char(*start); i++) {
                        cur = cur * 16 + (
                          (*start <= '9') ? 
                          (*start -  '0') : 
                          ((*start | 0x20) - 'a' + 10)
                        );
                        start++;
                      }
                      break;

                    default:
                      if (!is_oct_char(*start)) {
                        cur = *start++;
                        break;
                      }
                      for (std::size_t i = 0; i < 3 && is_oct_char(*start); i++) {
                        cur = cur * 8 + (*start - '0');
                        start++;
                      }
                  }
                  buffer.stuff_back(cur);
                  break;
                }
                default:
                  buffer.stuff_back(*start++);
              }        
            }
            prev = start;
          } else start += 8;
        }
      }  
    
    private:
      Buf buffer{};
  };

}