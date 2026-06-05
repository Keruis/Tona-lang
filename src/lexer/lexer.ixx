module;

#include "../tona.h"

export module tona.lexer;

import tona.types;
import tona.token;

import std;

#define PARSE_DOUBLE_CHAR(last_char, double_type, one_type) \
  if (*++cur == last_char) {         \
    cur++;                           \
    vec_toks.push_back(Token{        \
      .start_ptr = cur - 2,          \
      .type = TokenType::double_type \
    });                              \
  } else {                           \
    vec_toks.push_back(Token{        \
      .start_ptr = cur - 1,          \
      .type = TokenType::one_type    \
    });                              \
  }

export namespace Tona {

  class Lexer {
    public:
      [[nodiscard]] std::expected<TokenContext, TokenError> tokens(std::string_view path, std::string_view text) noexcept {
        TokenContext ctx;
        ctx.path = path;

        tccp cur = text.data();

        auto& vec_toks = ctx.tokens;
        vec_toks.reserve(text.size() / 5);

        tccp start_ptr = nullptr;
        TokenType num_type = TokenType::_;

        constexpr void* labels[256] = {
          #include "lexer_label.inc"
        };

        goto *labels[static_cast<std::uint8_t>(*cur)];
        
        l_skip:
        l_newline:
          do {
            cur++;
          } while (*cur == ' ' || *cur == '\t' || *cur == '\n');
          goto *labels[static_cast<std::uint8_t>(*cur)];
          
        l_identifier: {
          tccp end_ptr = identifier_char(cur);
          std::string_view identifier(cur, end_ptr);
          if (
            auto res = find_keyword(identifier); 
            res == TokenType::T_IDENTIFIER
          ) vec_toks.push_back(Token{
              .text = identifier,
              .start_ptr = cur,
              .type = res
            });
          else vec_toks.push_back(Token{
            .start_ptr = cur,
            .type = TokenType::T_IDENTIFIER
          });

          goto *labels[static_cast<std::uint8_t>(*(cur = end_ptr))];
        }

        l_op_chars:
        l_punc_chars:
          vec_toks.push_back(Token{
            .start_ptr = cur,
            .type = static_cast<TokenType>(*cur)
          });
          goto *labels[static_cast<std::uint8_t>(*++cur)];

        l_digit_0:
          start_ptr = cur++;
          switch (*cur) {
            case 'b':
            case 'B': cur++; goto pn_bin_prefix;
            case 'o':
            case 'O': cur++; goto pn_oct_prefix;
            case 'x':
            case 'X': cur++; goto pn_hex_prefix;
            case '.': cur++; goto pn_franction_direct;
            case 'e':
            case 'E': cur++; goto pn_exponect_direct;
            case 'u':
            case 'U':
            case 'i':
            case 'I': cur++; num_type = TokenType::T_LITERALS_INT; goto pn_suf_num;
            case 'f':
            case 'F': cur++; num_type = TokenType::T_LITERALS_FLOAT; goto pn_suf_num;
            default:
              if (is_dec_char(*cur)) [[unlikely]]
                goto ERR_INVALID_NUMERIC_LITERAL;
              vec_toks.push_back(Token{
                .text = std::string_view(start_ptr, 1),
                .start_ptr = start_ptr,
                .type = TokenType::T_LITERALS_INT
              });
          }

          goto *labels[static_cast<std::uint8_t>(*cur)];
          
        l_digit_1_9:
          start_ptr = cur++;
          cur = consume_digit_sequence<
            is_dec_char
          >(cur);
          switch (*cur) {
            case '.': cur++; goto pn_franction_direct;
            case 'e':
            case 'E': cur++; goto pn_exponect_direct;
            case 'u':
            case 'U':
            case 'i':
            case 'I': cur++; num_type = TokenType::T_LITERALS_INT; goto pn_suf_num;
            case 'f':
            case 'F': cur++; num_type = TokenType::T_LITERALS_FLOAT; goto pn_suf_num;
            default:
              if (is_identifier_char(*cur))
                goto ERR_INVALID_NUMERIC_LITERAL;
              vec_toks.push_back(Token{
                .text = std::string_view(start_ptr, cur),
                .start_ptr = start_ptr,
                .type = TokenType::T_LITERALS_INT
              });
          }

          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_string:
          start_ptr = ++cur;
          if ((cur = read_string(cur, text.end(), ctx.strings)))
            vec_toks.push_back(Token{
              .str_idx = ctx.strings.size() - 1,
              .start_ptr = start_ptr,
              .type = TokenType::T_LITERALS_INT
            });
          else goto ERR_UNTERMINATED_STRING;
          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_assign:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_EQ, T_OPERATORS_ASSIGN)
          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_less:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_LE, T_OPERATORS_LT)
          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_greater:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_GE, T_OPERATORS_GT)
          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_not:
          PARSE_DOUBLE_CHAR('=', T_OPERATORS_NEQ, T_OPERATORS_NOT)
          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_div:
          if (*++cur == '/') {
            do {
              cur++;
            } while (*cur != '\n' && *cur != '\0');
            goto *labels[static_cast<std::uint8_t>(*cur)];
          } else if (*cur == '*') {
            multi_comments:
            do {
              cur++;
            } while (*cur != '*');
            if (*++cur != '/')
              goto multi_comments;
            cur++;
          } else {
            vec_toks.push_back(Token{
              .start_ptr = cur - 1,
              .type = TokenType::T_OPERATORS_DIV
            });
          }

          goto *labels[static_cast<std::uint8_t>(*cur)];


        pn_bin_prefix:
          cur = consume_digit_sequence<
            bin_char
          >(cur);

          goto pn_end;
          
        pn_oct_prefix:
          cur = consume_digit_sequence<
            [](char c){
              return c >= '0' && c <= '7';
            }
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
            num_type = TokenType::T_LITERALS_FLOAT;
            if (*cur == '+' || *cur == '-')
              cur++;
            if (!is_dec_char(*cur))
              goto ERR_INVALID_NUMERIC_LITERAL;
            cur = consume_digit_sequence<
              is_dec_char
            >(cur);
          }

        pn_end:
          if (*cur == '\'') [[unlikely]]
            goto ERR_INVALID_NUMERIC_LITERAL;

        pn_suf_num:
          while (is_dec_char(*cur))
            cur++;

          vec_toks.push_back(Token{
            .text = std::string_view(start_ptr, cur),
            .start_ptr = start_ptr,
            .type = num_type
          });

          goto *labels[static_cast<std::uint8_t>(*cur)];

        l_default:
          return std::unexpected(TokenError{

          });

        l_end:
          vec_toks.push_back(Token{
            .start_ptr = cur,
            .type = TokenType::T_END
          });
          return ctx;

        ERR_INVALID_NUMERIC_LITERAL:
          return std::unexpected(TokenError{

          });

        ERR_UNTERMINATED_STRING:
          return std::unexpected(TokenError{

          });

        return ctx;
      }

    private:
      template <auto PredFunc, typename FT = decltype(PredFunc)>
      [[nodiscard]] [[gnu::always_inline]] tccp consume_digit_sequence(tccp start) noexcept {
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

      [[nodiscard]] [[gnu::always_inline]] tccp read_string(tccp start, tccp end, std::vector<std::string>& vec_str) noexcept {
        std::string str;
        start++;
        while (true) {
          std::size_t size = end - start;
          const char* next_quote = static_cast<const char*>(
            std::memchr(start, '"', size)
          );

          if (!next_quote) [[unlikely]]
            return nullptr;

          const char* next_backslash;
          next_backslash = static_cast<const char*>(
            std::memchr(start, '\\', next_quote - start)
          );

          if (!next_backslash) {
            str.append(start, next_quote - start);
            vec_str.push_back(std::move(str));
            return next_quote + 1;
          }

          str.append(start, next_backslash);
          start = next_backslash;
          
          char c;
          switch (*++start) {
            case 'a':  c = '\a'; break;
            case 'b':  c = '\b'; break;
            case 'f':  c = '\f'; break;
            case 'n':  c = '\n'; break;
            case 'r':  c = '\r'; break;
            case 't':  c = '\t'; break;
            case 'v':  c = '\v'; break;
            case '"':  c = '"';  break;
            case '0':  c = '\0'; break;
            default:   c = *start; break;
          }

          str.push_back(c);
          start++;
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
          t[static_cast<std::uint8_t>('_')] = true;
          return t;
        }();

        return table[static_cast<std::uint8_t>(c)];
      }

      [[nodiscard]] [[using gnu:always_inline]] inline tccp identifier_char(tccp start) noexcept {
        start++;
        while (reinterpret_cast<std::uintptr_t>(start) & 7) {
          if (!is_identifier_char(*start))
            return start;
          start++;
        }
        
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

          if (final_invalid_flags != 0) {
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

          if (final_invalid_flags != 0) {
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
  };

}