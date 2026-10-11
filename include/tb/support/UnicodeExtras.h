#ifndef __UNICODEEXTRAS__H__
#define __UNICODEEXTRAS__H__

#include "../../utility/Algorithms.h"

#include "../lex/UnicodeChar.h"

namespace tb::unicode {

constexpr bool IsUnicodeWhitespace(char32_t cp) noexcept {
  return utility::IsAnyOf(cp, U' ', U'\t', U'\v', U'\f');
}

constexpr bool IsUnicodeLineTerminator(char32_t cp) noexcept {
  return utility::IsAnyOf(cp, U'\n', U'\r', U'\u2028', U'\u2029', U'\u0085');
}

constexpr bool IsXidStart(char32_t cp) noexcept {
  UnicodeCharRange ranges(xid_start_range);
  return ranges.Contains(cp);
}

constexpr bool IsXidContinue(char32_t cp) noexcept {
  UnicodeCharRange ranges(xid_continue_range);
  return ranges.Contains(cp);
}

}

#endif