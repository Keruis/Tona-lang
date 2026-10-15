#ifndef __DIAGNOSTIC__H__
#define __DIAGNOSTIC__H__

#include <string>
#include <cstddef>
#include <string_view>

namespace tb {

struct Diagnostic {
  std::string message;
  std::size_t line = 1;
  std::size_t column = 1;
  std::string_view source_line{};

  std::string render() const;
};

}

#endif