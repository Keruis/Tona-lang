export module tona.lexer;

import std;

import tona.types;
import tona.token;
import tona.buf;

#define PARSE_DOUBLE_CHAR(last_char, double_type, one_type) \
  if (*++cur == last_char) {         \
    cur++;                           \
    emit({                           \
      .start = cur - 2,              \
      .type = TokenType::double_type \
    });                              \
  } else {                           \
    emit({                           \
      .start = cur - 1,              \
      .type = TokenType::one_type    \
    });                              \
  }

export namespace Tona {

  class Lexer {
    public:
      [[nodiscard]] TokenContext tokenize(std::string_view text) noexcept {
        TokenContext ctx;

        auto& vec_toks = ctx.tokens;
        vec_toks.resize(text.size() / 5);

        const char* cur = text.data();
        Token* dst = vec_toks.data();
        Token* end = vec_toks.data() + vec_toks.size();

        auto emit = [&][[gnu::always_inline]](Token&& tok) {
          if (dst == end) [[unlikely]]
            grow_tokens(vec_toks, dst, end);
          *dst++ = std::move(tok);
        };

        const char* start_ptr = nullptr;
        TokenType num_type = TokenType::T_LITERALS_INT;

        constexpr void* labels[256] = {
          #include "lexer_label.inc"
        };

        goto *labels[cast_u8(*cur)];
        
        l_skip:
        l_newline:
          do {
            cur++;
          } while (*cur == ' ' || *cur == '\t' || *cur == '\n');
          goto *labels[cast_u8(*cur)];
          
        l_identifier: {
          const char* const end_ptr = identifier_char(cur);
          std::string_view identifier(cur, end_ptr);
          if (
            auto res = find_keyword(identifier); 
            res == TokenType::T_IDENTIFIER
          ) emit({
              .text = {
                .data = cur,
                .len = identifier.size()
              },
              .start = cur,
              .type = res
            });
          else emit({
            .start = cur,
            .type = TokenType::T_IDENTIFIER
          });

          goto *labels[cast_u8(*(cur = end_ptr))];
        }

        l_op_chars:
        l_punc_chars:
          emit({
            .start = cur,
            .type = static_cast<TokenType>(*cur)
          });
          goto *labels[cast_u8(*++cur)];

        l_digit_0:
          start_ptr = cur++;
          switch (*cur) {
            case 'b' :
            case 'B' : cur++; goto pn_bin_prefix;
            case 'o' :
            case 'O' : cur++; goto pn_oct_prefix;
            case 'x' :
            case 'X' : cur++; goto pn_hex_prefix;
            case '.' : cur++; goto pn_franction_direct;
            case 'e' :
            case 'E' : cur++; 
              num_type = TokenType::T_LITERALS_FLOAT; 
              goto pn_exponect_direct;
            case 'u' :
            case 'U' :
            case 'i' :
            case 'I' : cur++; goto pn_suf_num_i;
            case 'f' :
            case 'F' : cur++; goto pn_suf_num_f;
            case '\'': goto ERR_INVALID_NUMERIC_LITERAL;
            default:
              if (is_dec_char(*cur)) [[unlikely]]
                goto ERR_INVALID_NUMERIC_LITERAL;
              emit({
                .text = {
                  .data = start_ptr, 
                  .len = 1
                },
                .start = start_ptr,
                .type = TokenType::T_LITERALS_INT
              });
          }

          goto *labels[cast_u8(*cur)];
          
        l_digit_1_9:
          start_ptr = cur++;
          cur = consume_digit_sequence<
            is_dec_char
          >(cur);
          switch (*cur) {
            case '.' : cur++; goto pn_franction_direct;
            case 'e' :
            case 'E' : cur++; 
              num_type = TokenType::T_LITERALS_FLOAT; 
              goto pn_exponect_direct;
            case 'u' :
            case 'U' :
            case 'i' :
            case 'I' : cur++; goto pn_suf_num_i;
            case 'f' :
            case 'F' : cur++; goto pn_suf_num_f;
            case '\'': goto ERR_INVALID_NUMERIC_LITERAL;
            default:
              if (is_identifier_char(*cur))
                goto ERR_INVALID_NUMERIC_LITERAL;
              emit({
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
          start_ptr = ++cur;
          if ((cur = read_string(cur, ctx.strings)))
            emit({
              .str_idx = ctx.strings.size() - 1,
              .start = start_ptr,
              .type = TokenType::T_LITERALS_STRING
            });
          else goto ERR_UNTERMINATED_STRING;
          goto *labels[cast_u8(*cur)];

        l_assign:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_EQ, T_OPERATORS_ASSIGN)
          goto *labels[cast_u8(*cur)];

        l_less:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_LE, T_OPERATORS_LT)
          goto *labels[cast_u8(*cur)];

        l_greater:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_GE, T_OPERATORS_GT)
          goto *labels[cast_u8(*cur)];

        l_not:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_NEQ, T_OPERATORS_NOT)
          goto *labels[cast_u8(*cur)];

        l_div:
          if (*++cur == '/') {
            do {
              cur++;
            } while (*cur != '\n' && *cur != '\0');
          } else if (*cur == '*') {
            while (true) {
              cur++;
              if (*cur == '\0') [[unlikely]]
                goto ERR_UNCLOSE_COMMENT;
              if (*cur == '*' && cur[1] == '/')
                break;
            }
            cur += 2;
          } else {
            emit({
              .start = cur - 1,
              .type = TokenType::T_OPERATORS_DIV
            });
          }

          goto *labels[cast_u8(*cur)];

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
          num_type = TokenType::T_LITERALS_FLOAT;
          cur = consume_digit_sequence<
            is_dec_char
          >(cur);

          if (*cur == 'e' || *cur == 'E') {
        pn_exponect_direct:
            if (*cur == '+' || *cur == '-')
              cur++;
            if (!is_dec_char(*cur))
              goto ERR_INVALID_NUMERIC_LITERAL;
            cur = consume_digit_sequence<
              is_dec_char
            >(cur);
          }

        pn_end:
          if (cur[-1] == '\'') [[unlikely]]
            goto ERR_INVALID_NUMERIC_LITERAL;
  
          switch (*cur) {
            case 'u':
            case 'U':
            case 'i':
        pn_suf_num_i:
            case 'I': 
              num_type = TokenType::T_LITERALS_INT_SUF; 
              break;
            case 'f':
        pn_suf_num_f:
            case 'F': 
              num_type = TokenType::T_LITERALS_FLOAT_SUF; 
              break;
            default: goto pn_save;
          }
        pn_suf_num:
          while (is_dec_char(*cur))
            cur++;
        pn_save:
          emit({
            .text = {
              .data = start_ptr, 
              .len = cast_usize(cur - start_ptr)
            },
            .start = start_ptr,
            .type = num_type
          });

          num_type = TokenType::T_LITERALS_INT;
          goto *labels[cast_u8(*cur)];

        l_end:
          emit({
            .start = cur,
            .type = TokenType::T_END
          });
          vec_toks.resize(dst - vec_toks.data());
          return ctx;

        l_default:

        ERR_INVALID_NUMERIC_LITERAL:

        ERR_UNTERMINATED_STRING:

        ERR_UNCLOSE_COMMENT:

        return ctx;
      }

    private:
      [[gnu::noinline]] void grow_tokens(std::vector<Token>& vec, Token*& dst, Token*& end) noexcept {
        std::size_t idx = dst - vec.data();
        vec.resize(vec.size() * 2);
        dst = vec.data() + idx;
        end = vec.data() + vec.size();
      }

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

      [[nodiscard]] [[gnu::always_inline]] inline const char* read_string(const char* start, std::vector<std::string>& vec_str) noexcept {
        constexpr std::uint64_t all_bytes_one     = 0x0101010101010101ULL;
        constexpr std::uint64_t msb_only_mask     = 0x80 * all_bytes_one;

        constexpr std::uint64_t quote_mask        = '"' * all_bytes_one;
        constexpr std::uint64_t escape_mask       = '\\' * all_bytes_one;
        constexpr std::uint64_t carriage_mask     = '\r' * all_bytes_one;
        constexpr std::uint64_t newline_mask      = '\n' * all_bytes_one;

        const char* prev = start;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8);

          std::uint64_t quote_match        = chunk_bytes ^ quote_mask;
          std::uint64_t escape_match       = chunk_bytes ^ escape_mask;
          std::uint64_t carriage_match     = chunk_bytes ^ carriage_mask;
          std::uint64_t newline_match      = chunk_bytes ^ newline_mask;

          std::uint64_t null_minus_one     = chunk_bytes - all_bytes_one;
          std::uint64_t quote_minus_one    = quote_match - all_bytes_one;
          std::uint64_t escape_minus_one   = escape_match - all_bytes_one;
          std::uint64_t carriage_minus_one = carriage_match - all_bytes_one;
          std::uint64_t newline_minus_one  = newline_match - all_bytes_one;

          std::uint64_t null_inverse       = ~chunk_bytes;
          std::uint64_t quote_inverse      = ~quote_match;
          std::uint64_t escape_inverse     = ~escape_match;
          std::uint64_t carriage_inverse   = ~carriage_match;
          std::uint64_t newline_inverse    = ~newline_match;

          std::uint64_t null_msb_match     = null_minus_one & null_inverse;
          std::uint64_t quote_msb_match    = quote_minus_one & quote_inverse;
          std::uint64_t escape_msb_match   = escape_minus_one & escape_inverse;
          std::uint64_t carriage_msb_match = carriage_minus_one & carriage_inverse;
          std::uint64_t newline_msb_match  = newline_minus_one & newline_inverse;

          std::uint64_t final_invalid_flags = (null_msb_match | quote_msb_match | escape_msb_match | carriage_msb_match | newline_msb_match) & msb_only_mask;

          if (final_invalid_flags) [[unlikely]] {
            std::size_t size;
            if constexpr (std::endian::native == std::endian::big)
              size = (std::countl_zero(final_invalid_flags) >> 3);
            else
              size = (std::countr_zero(final_invalid_flags) >> 3);
            start += size;
            buffer.stuff_back(prev, start - prev);
            const char* const end = start + 8 - size;
            while (start < end) {
              switch (*start) {
                case '"' :
                  vec_str.push_back(std::string(buffer.buffer<const char*>(), buffer.buf_size()));
                  buffer.reset();
                  return start + 1;
                case '\0':
                case '\n':
                case '\r': return nullptr;
                case '\\': {
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
                      goto check_out_range;

                    default:
                      if (!is_oct_char(*start)) {
                        cur = *start++;
                        break;
                      }
                      for (std::size_t i = 0; i < 3 && is_oct_char(*start); i++) {
                        cur = cur * 8 + (*start - '0');
                        start++;
                      }

                  check_out_range:
                      if (cur > std::numeric_limits<char>::max())
                        return nullptr;
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

        std::unreachable();
      }

    private:
      [[nodiscard]] [[gnu::always_inline]] inline bool is_identifier_char(char c) noexcept {
        constexpr auto table = []{
          std::array<bool, 256> t{};
          for (std::uint8_t c = 'a'; c <= 'z'; ++c) t[c] = true;
          for (std::uint8_t c = 'A'; c <= 'Z'; ++c) t[c] = true;
          for (std::uint8_t c = '0'; c <= '9'; ++c) t[c] = true;
          t[cast_u8('_')] = true;
          return t;
        }();

        return table[cast_u8(c)];
      }

      [[nodiscard]] [[gnu::always_inline]] inline const char* identifier_char(const char* start) noexcept {
        start++;
        
        constexpr std::uint64_t all_bytes_one     = 0x0101010101010101ULL;
        constexpr std::uint64_t clear_msb_mask    = 0x7F * all_bytes_one;
        constexpr std::uint64_t msb_only_mask     = 0x80 * all_bytes_one;
        constexpr std::uint64_t case_flatten_mask = 0x20 * all_bytes_one;

        constexpr std::uint64_t ge_a_adder        = (0x80 - 'a') * all_bytes_one;
        constexpr std::uint64_t gt_z_adder        = (0x7F - 'z') * all_bytes_one;
        constexpr std::uint64_t ge_0_adder        = (0x80 - '0') * all_bytes_one;
        constexpr std::uint64_t gt_9_adder        = (0x7F - '9') * all_bytes_one;
        constexpr std::uint64_t under_mask        = '_' * all_bytes_one;

        while (true) {
          std::uint64_t chunk_bytes; 
          std::memcpy(&chunk_bytes, start, 8);

          std::uint64_t raw_7bit_bytes       = chunk_bytes & clear_msb_mask;

          std::uint64_t flattened_case       = chunk_bytes | case_flatten_mask;
          std::uint64_t letters_7bit_bytes   = flattened_case & clear_msb_mask;
          
          std::uint64_t letter_ge_a_overflow = letters_7bit_bytes + ge_a_adder;
          std::uint64_t letter_gt_z_overflow = letters_7bit_bytes + gt_z_adder;
          std::uint64_t letter_msb_match     = letter_ge_a_overflow ^ letter_gt_z_overflow;

          std::uint64_t digit_ge_0_overflow  = raw_7bit_bytes + ge_0_adder;
          std::uint64_t digit_gt_9_overflow  = raw_7bit_bytes + gt_9_adder;
          std::uint64_t digit_msb_match      = digit_ge_0_overflow ^ digit_gt_9_overflow;

          std::uint64_t under_msb_match      = msb_only_mask - (raw_7bit_bytes ^ under_mask);

          std::uint64_t any_valid_msb = letter_msb_match | digit_msb_match | under_msb_match;
          
          std::uint64_t invalid_msb_mask = ~any_valid_msb;
          
          std::uint64_t combined_invalid_and_raw = invalid_msb_mask | chunk_bytes;
          
          std::uint64_t final_invalid_flags = combined_invalid_and_raw & msb_only_mask;

          if (final_invalid_flags) {
            if constexpr (std::endian::native == std::endian::big)
              return start + (std::countl_zero(final_invalid_flags) >> 3);
            else
              return start + (std::countr_zero(final_invalid_flags) >> 3);
          }
          
          start += 8;
        }
      }

      [[nodiscard]] [[gnu::always_inline]] inline static const char* bin_char(const char* start) noexcept {        
        constexpr std::uint64_t all_bytes_one    = 0x0101010101010101ULL;
        constexpr std::uint64_t not_all_byte_one = ~all_bytes_one;
        constexpr std::uint64_t clear_msb_mark   = 0x7f * all_bytes_one;
        constexpr std::uint64_t msb_only_mask    = 0x80 * all_bytes_one;

        constexpr std::uint64_t bin_mask         = 0x30 * all_bytes_one;
        constexpr std::uint64_t sep_mask         = '\'' * all_bytes_one;

        std::uint64_t prev_ends_with_sep = 0;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8); 

          std::uint64_t raw_7bit_bytes = chunk_bytes & clear_msb_mark;

          std::uint64_t bin_diff = (raw_7bit_bytes & not_all_byte_one) ^ bin_mask;
          std::uint64_t sep_diff = raw_7bit_bytes ^ sep_mask;

          std::uint64_t bin_valid = msb_only_mask - bin_diff;
          std::uint64_t sep_valid = msb_only_mask - sep_diff;

          std::uint64_t vaild_msb = ~(bin_valid | sep_valid);

          std::uint64_t final_invalid_flags = (vaild_msb | chunk_bytes) & msb_only_mask;

          std::uint64_t sep_bytes = sep_valid & msb_only_mask;
          std::uint64_t consecutive_sep = 0;
          std::uint64_t boundary_sep = prev_ends_with_sep & sep_bytes;

          if constexpr (std::endian::native == std::endian::big) {
            consecutive_sep = sep_bytes & (sep_bytes >> 8);
            prev_ends_with_sep = sep_bytes << 56;
          } else {
            consecutive_sep = sep_bytes & (sep_bytes << 8);
            prev_ends_with_sep = sep_bytes >> 56;
          }

          final_invalid_flags |= consecutive_sep | boundary_sep;

          if (final_invalid_flags) {
            if constexpr (std::endian::native == std::endian::big)
              return start + (std::countl_zero(final_invalid_flags) >> 3);
            else
              return start + (std::countr_zero(final_invalid_flags) >> 3);
          } 

          start += 8;
        }
      }

      [[nodiscard]] [[gnu::always_inline]] inline static bool is_hex_char(char c) noexcept {
        return c >= '0' && c <= '9' || (c | 0x20) >= 'a' && (c | 0x20) <= 'f';
      }
 
      [[nodiscard]] [[gnu::always_inline]] inline static bool is_dec_char(char c) noexcept {
        return c >= '0' && c <= '9';
      }

      [[nodiscard]] [[gnu::always_inline]] inline static bool is_oct_char(char c) noexcept {
        return c >= '0' && c <= '7';
      }
    
    private:
      TBuf buffer{};
  };

}