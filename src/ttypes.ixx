export module tona.types;

import std;

export namespace Tona {

  [[nodiscard]] constexpr std::uint8_t cast_u8(auto val) {
    return static_cast<std::uint8_t>(val);
  }

  [[nodiscard]] constexpr std::size_t cast_usize(auto val) {
    return static_cast<std::size_t>(val);
  }

}