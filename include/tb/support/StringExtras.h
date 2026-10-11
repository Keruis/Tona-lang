#ifndef __STRINGEXTRAS__H__
#define __STRINGEXTRAS__H__

#include "../../utility/Algorithms.h"

namespace tb {

enum Radix {
  Bin,
  Oct,
  Dec,
  Hex
};

constexpr bool IsWhitespace(char c) noexcept {
  return c == ' ' || utility::Interval<'\t', '\r'>::IsInClosedInterval(c);
}

constexpr bool IsUpper(char c) noexcept {
  return utility::Interval<'A', 'Z'>::IsInClosedInterval(c);
}

constexpr bool IsLower(char c) noexcept {
  return utility::Interval<'a', 'z'>::IsInClosedInterval(c);
}

constexpr bool IsAlpha(char c) noexcept {
  return IsLower(c) || IsUpper(c);
}

template <Radix r>
constexpr bool IsDigit(char c) noexcept {
  if constexpr (r == Hex) {
    return IsDigit<Dec>(c) || utility::Interval<'a', 'f'>::IsInClosedInterval(c | 0x20);
  } else if constexpr (r == Dec) {
    return utility::Interval<'0', '9'>::IsInClosedInterval(c);
  } else if constexpr (r == Oct) {
    return utility::Interval<'0', '7'>::IsInClosedInterval(c);
  } else return utility::IsAnyOf(c, '0', '1');
}

constexpr bool IsAlnum(char c) noexcept {
  return IsAlpha(c) || IsDigit<Dec>(c);
}

constexpr bool IsAscii(char32_t c) noexcept {
  return c <= 0x7F;
}

}

#endif