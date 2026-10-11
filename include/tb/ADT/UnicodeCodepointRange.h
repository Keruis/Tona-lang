#ifndef __UNICODECODEPOINTRANGE__H__
#define __UNICODECODEPOINTRANGE__H__

#include <span>
#include <algorithm>

namespace tb {

class UnicodeCharRange {
  public:
    struct CharRanges {
      char32_t start{};
      char32_t end{};
    };

  public:
    constexpr UnicodeCharRange(std::span<const CharRanges> ranges)
      : ranges(ranges) {}

    constexpr bool Contains(char32_t cp) const noexcept {
      auto it = std::ranges::lower_bound(ranges, cp, {}, &CharRanges::end);
      return it != ranges.end() && it->start <= cp;
    }

  private:
    std::span<const CharRanges> ranges;
};

}

#endif