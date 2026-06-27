export module tona.byte;

import std;

export namespace Tona {

  [[nodiscard]] constexpr std::uint8_t cast_u8(auto val) {
    return static_cast<std::uint8_t>(val);
  }

  [[nodiscard]] constexpr std::size_t cast_usize(auto val) {
    return static_cast<std::size_t>(val);
  }

  struct SWAR64 {
    static constexpr std::uint64_t all_bytes_one = 0x0101010101010101ULL;
    static constexpr std::uint64_t not_all_byte_one = ~all_bytes_one;
    static constexpr std::uint64_t msb_only_mask = 0x80 * all_bytes_one;
    static constexpr std::uint64_t clear_msb_mask = 0x7F * all_bytes_one;

    template <std::uint8_t c>
    static constexpr std::uint64_t char_mask = c * all_bytes_one;
    template <std::uint8_t c>
      requires (c < 0x80)
    struct Range {
      static constexpr std::uint64_t ge_addr = (0x80 - c) * all_bytes_one;
      static constexpr std::uint64_t gt_addr = (0x7F - c) * all_bytes_one;
    };
  };

  [[nodiscard]] [[gnu::always_inline]] inline std::uint64_t match_offset(std::uint64_t msb) noexcept {
    if constexpr (std::endian::native == std::endian::big)
      return std::countl_zero(msb) >> 3;
    else
      return std::countr_zero(msb) >> 3;
  }

  [[nodiscard]] [[gnu::always_inline]] inline const char* skip_whitespace(const char* cur) noexcept {
    cur++;

    while (true) {
      std::uint64_t chunk_bytes;
      std::memcpy(&chunk_bytes, cur, 8);

      const std::uint64_t raw_7bit_bytes = chunk_bytes & SWAR64::clear_msb_mask;

      const std::uint64_t space_diff   = raw_7bit_bytes ^ SWAR64::char_mask<' '>;
      const std::uint64_t tab_diff     = raw_7bit_bytes ^ SWAR64::char_mask<'\t'>;
      const std::uint64_t newline_diff = raw_7bit_bytes ^ SWAR64::char_mask<'\n'>;

      const std::uint64_t space_valid = SWAR64::msb_only_mask - space_diff;
      const std::uint64_t tab_valid = SWAR64::msb_only_mask - tab_diff;
      const std::uint64_t newline_valid = SWAR64::msb_only_mask - newline_diff;

      const std::uint64_t vaild_msb = ~(space_valid | tab_valid | newline_valid);

      std::uint64_t final_invalid_flags = (vaild_msb | chunk_bytes) & SWAR64::msb_only_mask;

      if (final_invalid_flags) [[likely]]
        return cur + match_offset(final_invalid_flags);
      
      cur += 8;
    }
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

      const std::uint64_t raw_7bit_bytes       = chunk_bytes & SWAR64::clear_msb_mask;

      const std::uint64_t flattened_case       = chunk_bytes | SWAR64::char_mask<0x20>;
      const std::uint64_t letters_7bit_bytes   = flattened_case & SWAR64::clear_msb_mask;
      
      const std::uint64_t letter_ge_a_overflow = letters_7bit_bytes + SWAR64::Range<'a'>::ge_addr;
      const std::uint64_t letter_gt_z_overflow = letters_7bit_bytes + SWAR64::Range<'z'>::gt_addr;
      const std::uint64_t letter_msb_match     = letter_ge_a_overflow ^ letter_gt_z_overflow;

      const std::uint64_t digit_ge_0_overflow  = raw_7bit_bytes + SWAR64::Range<'0'>::ge_addr;
      const std::uint64_t digit_gt_9_overflow  = raw_7bit_bytes + SWAR64::Range<'9'>::gt_addr;
      const std::uint64_t digit_msb_match      = digit_ge_0_overflow ^ digit_gt_9_overflow;

      const std::uint64_t under_msb_match      = SWAR64::msb_only_mask - (raw_7bit_bytes ^ SWAR64::char_mask<'_'>);

      const std::uint64_t any_valid_msb = letter_msb_match | digit_msb_match | under_msb_match;
      
      const std::uint64_t invalid_msb_mask = ~any_valid_msb;
      
      const std::uint64_t combined_invalid_and_raw = invalid_msb_mask | chunk_bytes;
      
      const std::uint64_t final_invalid_flags = combined_invalid_and_raw & SWAR64::msb_only_mask;

      if (final_invalid_flags)
        return start + match_offset(final_invalid_flags);
      
      start += 8;
    }
  }

  [[nodiscard]] [[gnu::always_inline]] inline const char* bin_char(const char* start) noexcept {        
    std::uint64_t prev_ends_with_sep = 0;

    while (true) {
      std::uint64_t chunk_bytes;
      std::memcpy(&chunk_bytes, start, 8); 

      const std::uint64_t raw_7bit_bytes = chunk_bytes & SWAR64::clear_msb_mask;

      const std::uint64_t bin_diff = (raw_7bit_bytes & SWAR64::not_all_byte_one) ^ SWAR64::char_mask<0x30>;
      const std::uint64_t sep_diff = raw_7bit_bytes ^ SWAR64::char_mask<'\''>;

      const std::uint64_t bin_valid = SWAR64::msb_only_mask - bin_diff;
      const std::uint64_t sep_valid = SWAR64::msb_only_mask - sep_diff;

      const std::uint64_t vaild_msb = ~(bin_valid | sep_valid);

      std::uint64_t final_invalid_flags = (vaild_msb | chunk_bytes) & SWAR64::msb_only_mask;

      const std::uint64_t sep_bytes = sep_valid & SWAR64::msb_only_mask;
      std::uint64_t consecutive_sep = 0;
      const std::uint64_t boundary_sep = prev_ends_with_sep & sep_bytes;

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

  [[nodiscard]] [[gnu::always_inline]] inline bool is_hex_char(char c) noexcept {
    return (c >= '0' && c <= '9') || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
  }

  [[nodiscard]] [[gnu::always_inline]] inline bool is_dec_char(char c) noexcept {
    return c >= '0' && c <= '9';
  }

  [[nodiscard]] [[gnu::always_inline]] inline bool is_oct_char(char c) noexcept {
    return c >= '0' && c <= '7';
  }

}