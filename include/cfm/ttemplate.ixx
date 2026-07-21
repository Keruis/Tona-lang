export module tona.template_utils;

import std;

export namespace Tona {

  namespace Utils {
  
    template <typename T, T... args>
    bool any_of(T val) {
      return (all_of<T, args>(val) || ...);
    }

    template <typename T, T... args>
    bool all_of(T val) {
      return ((val == args) && ...);
    }
    
  }

}