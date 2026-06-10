export module tona.lexer;

import std;

import tona.token;
import tona.buf;
import tona.byte;

export namespace Tona {

  class Lexer {
    public:
      [[nodiscard]] TokenContext tokenize(std::string_view text) noexcept {
        TokenContext ctx;

        auto& vec_toks = ctx.tokens;
        vec_toks.reserve(text.size() / 5);

        const char* cur = text.data();

        auto emit = [&][[gnu::always_inline]](Token&& tok) {
          vec_toks.push_back(std::move(tok));
        };

        const char* start_ptr = nullptr;
        TokenType num_type = TokenType::T_LITERALS_INT;

        constexpr void* labels[256] = {
          #include "lexer_label.inc"
        };

        goto *labels[cast_u8(*cur)];
        
        l_skip: // ' '
        l_newline: // '\n'
          do {
            cur++;
          } while (*cur == ' ' || *cur == '\t' || *cur == '\n');
          goto *labels[cast_u8(*cur)];
          
        l_identifier: { // a - z A - Z _
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

        l_op_chars: // ! % * + - /
        l_punc_chars: // () [] : ; {}
          emit({
            .start = cur,
            .type = static_cast<TokenType>(*cur)
          });
          goto *labels[cast_u8(*++cur)];

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
            case '\'': goto ERR_INVALID_NUMERIC_LITERAL;
            default:
              if (is_identifier_char(*cur)) [[unlikely]]
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

        l_string: // "
          start_ptr = ++cur;
          if ((cur = read_string(cur, ctx.strings)))
            emit({
              .str_idx = ctx.strings.size() - 1,
              .start = start_ptr,
              .type = TokenType::T_LITERALS_STRING
            });
          else goto ERR_UNTERMINATED_STRING;
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
          const char* const start_ptr = cur;
          if (cur[1] == '=') {
            cur += 2;
            emit({
              .start = start_ptr,
              .type = double_type
            });
          } else {
            cur++;
            emit({
              .start = start_ptr,
              .type = static_cast<TokenType>(
                static_cast<std::uint8_t>(double_type) - double_char_offset
              )
            });
          }
        }
          goto *labels[cast_u8(*cur)];

        l_div: // / // /* */
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

        // chu li float
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
            if (!is_dec_char(*cur))
              goto ERR_INVALID_NUMERIC_LITERAL;
            cur = consume_digit_sequence<
              is_dec_char
            >(cur);
          }
          num_type = TokenType::T_LITERALS_FLOAT;

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

          return ctx;

        // zan shi ma you xian hao zen me chu li error
        l_default:

        ERR_INVALID_NUMERIC_LITERAL:

        ERR_UNTERMINATED_STRING:

        ERR_UNCLOSE_COMMENT:

        std::unreachable();
      }

    private:
      [[gnu::noinline]] void grow_tokens(std::vector<Token>& vec, Token*& dst, Token*& end) noexcept {
        std::size_t idx = dst - vec.data();
        vec.reserve(vec.capacity() * 2);
        dst = vec.data() + idx;
        end = vec.data() + vec.capacity();
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
        const char* prev = start;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8);

          std::uint64_t quote_match        = chunk_bytes ^ Byte::char_mask<'"'>;
          std::uint64_t escape_match       = chunk_bytes ^ Byte::char_mask<'\\'>;
          std::uint64_t carriage_match     = chunk_bytes ^ Byte::char_mask<'\r'>;
          std::uint64_t newline_match      = chunk_bytes ^ Byte::char_mask<'\n'>;

          std::uint64_t null_minus_one     = chunk_bytes - Byte::all_bytes_one;
          std::uint64_t quote_minus_one    = quote_match - Byte::all_bytes_one;
          std::uint64_t escape_minus_one   = escape_match - Byte::all_bytes_one;
          std::uint64_t carriage_minus_one = carriage_match - Byte::all_bytes_one;
          std::uint64_t newline_minus_one  = newline_match - Byte::all_bytes_one;

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

          std::uint64_t final_invalid_flags = (null_msb_match | quote_msb_match | escape_msb_match | carriage_msb_match | newline_msb_match) & Byte::msb_only_mask;

          if (final_invalid_flags) [[unlikely]] {
            std::size_t size = match_offset(final_invalid_flags);
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
                      if (cur > std::numeric_limits<std::uint8_t>::max())
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
      }

    private:
      [[nodiscard]] [[gnu::always_inline]] static inline std::uint64_t match_offset(std::uint64_t msb) noexcept {
        if constexpr (std::endian::native == std::endian::big)
          return std::countl_zero(msb) >> 3;
        else
          return std::countr_zero(msb) >> 3;
      }

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
      
        while (true) {
          std::uint64_t chunk_bytes; 
          std::memcpy(&chunk_bytes, start, 8);

          std::uint64_t raw_7bit_bytes       = chunk_bytes & Byte::clear_msb_mask;

          std::uint64_t flattened_case       = chunk_bytes | Byte::char_mask<0x20>;
          std::uint64_t letters_7bit_bytes   = flattened_case & Byte::clear_msb_mask;
          
          std::uint64_t letter_ge_a_overflow = letters_7bit_bytes + Byte::Range<'a'>::ge_addr;
          std::uint64_t letter_gt_z_overflow = letters_7bit_bytes + Byte::Range<'z'>::gt_addr;
          std::uint64_t letter_msb_match     = letter_ge_a_overflow ^ letter_gt_z_overflow;

          std::uint64_t digit_ge_0_overflow  = raw_7bit_bytes + Byte::Range<'0'>::ge_addr;
          std::uint64_t digit_gt_9_overflow  = raw_7bit_bytes + Byte::Range<'9'>::gt_addr;
          std::uint64_t digit_msb_match      = digit_ge_0_overflow ^ digit_gt_9_overflow;

          std::uint64_t under_msb_match      = Byte::msb_only_mask - (raw_7bit_bytes ^ Byte::char_mask<'_'>);

          std::uint64_t any_valid_msb = letter_msb_match | digit_msb_match | under_msb_match;
          
          std::uint64_t invalid_msb_mask = ~any_valid_msb;
          
          std::uint64_t combined_invalid_and_raw = invalid_msb_mask | chunk_bytes;
          
          std::uint64_t final_invalid_flags = combined_invalid_and_raw & Byte::msb_only_mask;

          if (final_invalid_flags)
            return start + match_offset(final_invalid_flags);
          
          start += 8;
        }
      }

      [[nodiscard]] [[gnu::always_inline]] inline static const char* bin_char(const char* start) noexcept {        
        std::uint64_t prev_ends_with_sep = 0;

        while (true) {
          std::uint64_t chunk_bytes;
          std::memcpy(&chunk_bytes, start, 8); 

          std::uint64_t raw_7bit_bytes = chunk_bytes & Byte::clear_msb_mask;

          std::uint64_t bin_diff = (raw_7bit_bytes & Byte::not_all_byte_one) ^ Byte::char_mask<0x30>;
          std::uint64_t sep_diff = raw_7bit_bytes ^ Byte::char_mask<'\''>;

          std::uint64_t bin_valid = Byte::msb_only_mask - bin_diff;
          std::uint64_t sep_valid = Byte::msb_only_mask - sep_diff;

          std::uint64_t vaild_msb = ~(bin_valid | sep_valid);

          std::uint64_t final_invalid_flags = (vaild_msb | chunk_bytes) & Byte::msb_only_mask;

          std::uint64_t sep_bytes = sep_valid & Byte::msb_only_mask;
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

          if (final_invalid_flags)
            return start + match_offset(final_invalid_flags);

          start += 8;
        }
      }

      [[nodiscard]] [[gnu::always_inline]] inline static bool is_hex_char(char c) noexcept {
        return (c >= '0' && c <= '9') || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
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