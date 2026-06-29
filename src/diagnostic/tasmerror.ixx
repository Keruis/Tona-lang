module;

#include <cstdlib>

export module tona.asm_err;

import std;

export namespace Tona {

  [[noreturn]] void asm_err() {
    std::exit(EXIT_FAILURE);
  }

}