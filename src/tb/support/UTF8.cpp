#include "../../../include/tb/support/UTF8.h"

#include "../../../include/utility/Algorithms.h"

bool DecodeAt(const char* start, const char* end, char32_t& cp, std::size_t& length) noexcept {
  const auto first = static_cast<std::uint8_t>(*start);
  switch (first) {
    case 0x00 ... 0x7F:
      length = 1;
      cp = first;
      return true;
    case 0xC2 ... 0xDF: {
      if (end - start < 2)
        return false;
      const auto second = static_cast<std::uint8_t>(start[1]);
      if ((second & 0xC0) != 0x80)
        return false;
      length = 2;
      cp = ((first & 0x1F) << 6) | (second & 0x3F);
      return true;
    }
    case 0xE0 ... 0xEF: {
      if (end - start < 3)
        return false;
      const auto second = static_cast<std::uint8_t>(start[1]);
      const auto third = static_cast<std::uint8_t>(start[2]);
      if (!utility::IsAllOf(0x80, second & 0xC0, third & 0xC0) || (first == 0xED && second > 0x9F))
        return false;
      length = 3;
      cp = ((first & 0x0F) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
      return true;
    }
    case 0xF0 ... 0xF4: {
      if (end - start < 4)
        return false;
      const auto second = static_cast<std::uint8_t>(start[1]);
      const auto third = static_cast<std::uint8_t>(start[2]);
      const auto fourth = static_cast<std::uint8_t>(start[3]);
      if (!utility::IsAllOf(0x80, second & 0xC0, third & 0xC0, fourth & 0xC0) || (first == 0xF0 && second > 0x90) || (first == 0xF4 && second > 0x8F))
        return false;
      length = 4;
      cp = ((first & 0x07) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3F);
      return true;
    }
    default:
      return false;
  }
}