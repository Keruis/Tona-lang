#include <cstddef>

#ifndef __LOCATION__H__
#define __LOCATION__H__

namespace tb {

struct Location {
  std::size_t line = 1;
  std::size_t column = 1;
  std::size_t length = 0;
};

}

#endif