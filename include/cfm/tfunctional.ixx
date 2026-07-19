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

}