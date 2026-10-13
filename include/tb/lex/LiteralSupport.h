#ifndef __LITERALSUPPORT__H__
#define __LITERALSUPPORT__H__

#include <cstdint>
#include <string_view>

namespace tb {

bool StringToNumeric(std::string_view text, char32_t& cp, std::size_t& length) noexcept;
  
}

#endif