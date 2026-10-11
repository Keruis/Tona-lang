#ifndef __UTF8__H__
#define __UTF8__H__

#include <cstdint>

namespace tb::unicode {

bool DecodeAt(const char* start, const char* end, char32_t& cp, std::size_t& length) noexcept;

}


#endif