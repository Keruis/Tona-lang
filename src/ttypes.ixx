export module tona.types;

import std;

export namespace Tona {

  using u8 = std::uint8_t;
  using u64 = std::uint64_t;
  using usize = std::size_t;
  using t_c = char;
  using t_byte = u8;
  using t_lable = void*;

  using t_cp = t_c*;
  using t_rcp = const t_c*;
  using t_crcp = const t_c* const;

  using t_bytes = t_byte*;
  using t_rbytes = const t_byte*;

  using t_cursor = t_rcp;

  [[nodiscard]] constexpr u8 cast_u8(auto val) {
    return static_cast<u8>(val);
  }

  [[nodiscard]] constexpr usize cast_usize(auto val) {
    return static_cast<usize>(val);
  }

}



