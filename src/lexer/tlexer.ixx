export module tona.lexer;

import std;

import tona.buf;
import tona.byte;
import tona.token;
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
        
        l_skip:
        l_newline:
          goto *labels[cast_u8(*(cur = skip_whitespace(cur)))];
          
        l_identifier:
          goto *labels[cast_u8(parse_identifier(cur, ctx))];

        l_op_chars:
        l_punc_chars:
          goto *labels[cast_u8(parse_single_char(cur, ctx))];

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
              goto pn_suf_num_f;
            [[unlikely]] case '\'': return std::unexpected(
              make_error(
                LexErrorType::LET_INVALID_DIGIT_SEPARATOR,
                start_ptr, cur
              )
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

        l_string:
          start_ptr = cur;
          if (!read_string(++cur, ctx, arena)) [[unlikely]]
            return std::unexpected(
              make_error(
                LexErrorType::LET_UNTERMINATED_STRING,
                start_ptr, cur
              )
            );

          goto *labels[cast_u8(*cur)];

        l_not:
        l_less:
        l_assign:
        l_greater:
          goto *labels[cast_u8(parse_double_char(cur, ctx))];

        l_div:
          if (!parse_div(cur, ctx)) [[unlikely]]
            return std::unexpected(
              make_error(
                LexErrorType::LET_UNCLOSE_COMMENT, 
                cur, 1
              )
            );

          goto *labels[cast_u8(*cur)];

        pn_bin_prefix:
          if (!parse_radix_digits<is_bin_char, bin_char>(cur)) [[unlikely]]
            goto pn_error;

          goto pn_end;
          
        pn_oct_prefix:
          if (!parse_radix_digits<is_oct_char, is_oct_char>(cur)) [[unlikely]]
            goto pn_error;

          goto pn_end;

        pn_hex_prefix:
          if (!parse_radix_digits<is_hex_char, is_hex_char>(cur)) [[unlikely]]
            goto pn_error;

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
              goto pn_error;
            consume_digit_sequence<
              is_dec_char
            >(cur);
          }
          num_type = TokenType::T_LITERALS_FLOAT;

        pn_end:
          if (*cur == '\'') [[unlikely]]
            goto pn_error;
  
          switch (*cur) {
            case 'u': case 'U':
            case 'i': case 'I':
        pn_suf_num_i:
              if (num_type != TokenType::T_LITERALS_INT) [[unlikely]]
                goto pn_error;
            case 'f': case 'F':
        pn_suf_num_f:
              cur++;
              num_type = TokenType::T_LITERALS_FLOAT;
              num_type = static_cast<TokenType>(cast_u8(num_type) + 2); 
              goto pn_suf_num;
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
            make_error(
              LexErrorType::LET_INVALID_NUMERIC_LITERAL,
              start_ptr, cur
            )
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
      [[nodiscard]] [[gnu::always_inline]] inline char parse_identifier(const char*& cur, TokenContext& ctx) {
        std::string_view identifier(cur, identifier_char(cur));
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
        cur = identifier.cend();
        return *cur;
      }

      [[nodiscard]] [[gnu::always_inline]] inline bool parse_div(const char*& cur, TokenContext& ctx) {
        if (*++cur == '/') {
          do {
            cur++;
          } while (*cur != '\n' && *cur != '\0');
        } else if (*cur == '*') {
          while (true) {
            cur++;
            if (*cur == '\0') [[unlikely]]
              return false;
            if (*cur == '*' && cur[1] == '/')
              break;
          }
          cur += 2;
        } else ctx.tokens.push_back({
            .start = cur - 1,
            .type = TokenType::T_OPERATORS_DIV
          });
        return true;
      } 

      [[nodiscard]] [[gnu::always_inline]] inline char parse_single_char(const char*& cur, TokenContext& ctx) {
        ctx.tokens.push_back({
          .start = cur,
          .type = static_cast<TokenType>(*cur)
        });
        cur++;
        return *cur;
      }

      [[nodiscard]] [[gnu::always_inline]] inline char parse_double_char(const char*& cur, TokenContext& ctx) {
        if (cur[1] == '=') {
          ctx.tokens.push_back({
            .start = cur,
            .type = static_cast<TokenType>(*cur + double_char_offset)
          });
          cur += 2;
        } else {
          ctx.tokens.push_back({
            .start = cur,
            .type = static_cast<TokenType>(*cur)
          });
          cur++;
        }
        return *cur;
      }

      [[nodiscard]] LexError parse_invalid_char(const char* cur) noexcept {
        std::size_t len = std::countl_one(static_cast<std::uint8_t>(*cur));
        len = (len == 0 || len > 4) ? 1 : len;
        return make_error(
          LexErrorType::LET_INVALID_CHAR,
          cur, len
        );
      }

      template <typename... Args>
        requires (std::constructible_from<std::string_view, Args...>)
      [[nodiscard]] LexError make_error(LexErrorType type, Args&&... args) noexcept {
        return {
          .err_text = std::string_view(std::forward<Args>(args)...),
          .type = type
        };
      }

    private:
      template <auto PredFunc, typename FT = decltype(PredFunc)>
      [[gnu::always_inline]] inline void consume_digit_sequence(const char*& start) noexcept {
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

      template <auto PredFunc1, auto PredFunc2, typename FT1 = decltype(PredFunc1)>
      [[nodiscard]] [[gnu::always_inline]] inline bool parse_radix_digits(const char*& start)
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

      [[nodiscard]] [[gnu::always_inline]] inline bool read_string(const char*& start, TokenContext& ctx, Arena& arena) {
        const char* const start_ptr = start;
        const char* prev_ptr = start;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8);

          const std::uint64_t ctrl_diff   = chunk_bytes - SWAR64::char_mask<0x20>;
          const std::uint64_t quote_diff  = chunk_bytes ^ SWAR64::char_mask<'"'>;
          const std::uint64_t escape_diff = chunk_bytes ^ SWAR64::char_mask<'\\'>;

          const std::uint64_t quote_minus_one  = quote_diff - SWAR64::all_bytes_one;
          const std::uint64_t escape_minus_one = escape_diff - SWAR64::all_bytes_one;

          const std::uint64_t final_invalid_flags = (ctrl_diff | quote_minus_one | escape_minus_one) & ~chunk_bytes & SWAR64::msb_only_mask;

          if (final_invalid_flags) [[unlikely]] {
            const char* end_ptr = start + 8;
            const std::size_t offset = match_offset(final_invalid_flags);
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
                ctx.tokens.push_back({
                  .text = {
                    .data = arena_mem,
                    .len = len
                  },
                  .start = start_ptr,
                  .type = TokenType::T_LITERALS_STRING
                });
                buffer.reset();
                return start + 1;
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
                        cur = cur * 16 + (
                          (*start <= '9') ? 
                          (*start -  '0') : 
                          ((*start | 0x20) - 'a' + 10)
                        );
                      }
                      break;
                    default:
                      start--;
                      if (std::size_t i = 2; is_oct_char(*start)) {
                        do {
                          cur = cur * 8 + (*start++ - '0');
                        } while (i-- && is_oct_char(*start));
                      } else cur = *start++;
                  }
                  buffer.stuff_back(cur);
                  break;
                }
                [[unlikely]] case '\0':
                [[unlikely]] case '\n':
                [[unlikely]] case '\r': return false;
                default:
                  buffer.stuff_back(*start++);
              }

              prev_ptr = start;
            }
          } else start += 8;
        }
      }
    
    private:
      Buf buffer{};
  };

}