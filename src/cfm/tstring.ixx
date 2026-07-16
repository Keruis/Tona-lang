export module tona.string;

import std;

export namespace Tona {

  struct StringView {
    const char* data;
    std::size_t len;
  };

}