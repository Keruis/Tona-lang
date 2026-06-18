export module tona.functional;

import std;

export namespace Tona {

  struct maximum {
    constexpr auto operator()(const auto& a, const auto& b) const {
      return std::max(a, b);
    }
  };

  struct minimum {
    constexpr auto operator()(const auto& a, const auto& b) const {
      return std::min(a, b);
    }
  };

  struct inc {
    constexpr auto operator()(const auto& a) const {
      return a + 1;
    }
  };

  struct dec {
    constexpr auto operator()(const auto& a) const {
      return a - 1;
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

  struct sign_extend_8 {
    constexpr auto operator()(const auto& a) const {
      return static_cast<std::int64_t>(static_cast<std::int8_t>(a));
    }
  };

  struct sign_extend_16 {
    constexpr auto operator()(const auto& a) const {
      return static_cast<std::int64_t>(static_cast<std::int16_t>(a));
    }
  };

  struct sign_extend_32 {
    constexpr auto operator()(const auto& a) const {
      return static_cast<std::int64_t>(static_cast<std::int32_t>(a));
    }
  };

}