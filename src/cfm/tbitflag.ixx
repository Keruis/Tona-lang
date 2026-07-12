export module tona.bitflag;

import std;

export namespace Tona {

  template <typename E>
    requires (std::is_enum_v<E>)
  class BitFlag {
    public:
      using UnderlyType = std::underlying_type_t<E>;

      constexpr BitFlag(E val = E{})
        noexcept : val(static_cast<UnderlyType>(val))
      {}

      constexpr void add(E val_) noexcept {
        val |= static_cast<UnderlyType>(val_);
      }

      [[nodiscard]] constexpr bool add_if(E val) noexcept {
        bool res = has(val);
        add(val);
        return res;
      }

      constexpr void sub(E val_) noexcept {
        val &= ~static_cast<UnderlyType>(val_);
      }

      [[nodiscard]] constexpr bool has(E val_) noexcept {
        return (val & static_cast<UnderlyType>(val_)) != 0; 
      }

      [[nodiscard]] constexpr E value() const noexcept {
        return static_cast<E>(val);
      }

    private:
      UnderlyType val;
  };

}