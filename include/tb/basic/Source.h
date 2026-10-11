#ifndef __SOURCE__H__
#define __SOURCE__H__

#include "../support/UnicodeExtras.h"

#include <cstdint>
#include <string_view>

namespace tb {

std::string_view source_line(std::string_view, std::size_t) noexcept;

}

#endif