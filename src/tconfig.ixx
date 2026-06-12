export module tona.config;

import std;

export namespace Tona {
  
  // tbuffer.ixx
  constexpr std::size_t TBUF_INIT_SIZE = 256;
  constexpr float TBUF_GROWTH_FACTOR = 1.5;

  // vm.ixx
  constexpr std::size_t TVM_MAX_REG_SIZE = 1024;
}