#ifndef __LITERALSUPPORT__H__
#define __LITERALSUPPORT__H__

#include "support/StringExtras.h"

namespace tb {

template <Radix r>
const char* ScanNumeric(const char* start, const char* end) noexcept {
  auto cur = start;
  while (cur <= end) {
    if (!IsDigit<r>(*cur))
      break;
  }
  return cur;
}
  
}

#endif