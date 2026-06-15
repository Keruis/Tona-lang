export module tona.byte;

import std;

export namespace Tona {

  [[nodiscard]] constexpr std::uint8_t cast_u8(auto val) {
    return static_cast<std::uint8_t>(val);
  }

  [[nodiscard]] constexpr std::size_t cast_usize(auto val) {
    return static_cast<std::size_t>(val);
  }

  struct Byte {
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

  struct maximum {
    constexpr auto operator()(const auto& a, const auto&& b) const {
      return std::max(a, b);
    }
  };

  struct minimum {
    constexpr auto operator()(const auto& a, const auto& b) const {
      return std::min(a, b);
    }
  };

  struct square_root {
    constexpr auto operator()(const auto& a) const {
      return std::sqrt(a);
    }
  };

  struct bit_shift_left {
    constexpr auto operator()(const auto& a, const auto& b) const {
      return a << b;
    }
  };

  struct bit_shift_right {
    constexpr auto operator()(const auto& a, const auto& b) const {
      return a >> b;
    }
  };

}