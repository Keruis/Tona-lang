#include "../../../include/tb/basic/Source.h"

namespace tb {

std::string_view source_line(std::string_view source, std::size_t line) noexcept {
  std::size_t current = 1;
  std::size_t begin = 0;
  for (std::size_t i = 0; i <= source.size(); ++i) {
    if (i == source.size() || unicode::IsUnicodeLineTerminator(source[i])) {
      if (current == line) {
        std::size_t end = i;
        if (end > begin && source[end - 1] == '\r')
          --end;
        return source.substr(begin, end - begin);
      }
      ++current;
      if (i + 1 < source.size() && source[i] == '\r' && source[i + 1] == '\n')
        ++i;
      begin = i + 1;
    }
  }
  return {};
}

}