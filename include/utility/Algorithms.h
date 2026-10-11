#ifndef __ALGORITHM__H__
#define __ALGORITHM__H__

#include <cstddef>
#include <functional>

namespace utility {

template <auto pred = std::equal_to<>{}, typename... Ts>
constexpr bool IsAnyOf(const auto& value, const Ts&&... candidates) noexcept((noexcept(pred(value, candidates)) && ...)) {
  return (pred(value, candidates) || ...);
}

template <auto pred = std::equal_to<>{}, typename... Ts>
constexpr bool IsAllOf(const auto& value, const Ts&&... candidates) noexcept((noexcept(pred(value, candidates)) && ...)) {
  return (pred(value, candidates) && ...);
}

template <std::size_t left, std::size_t right>
struct Interval {
  constexpr static bool IsInClosedInterval(std::size_t value) noexcept {
    return value >= left && value <= right; 
  }
  
  constexpr static bool IsInOpeninterval(std::size_t value) noexcept {
    return value > left && value < right;
  }

  constexpr static bool IsInLeftClosedInterval(std::size_t value) noexcept {
    return value >= left && value < right; 
  }
  
  constexpr static bool IsInLeftOpeninterval(std::size_t value) noexcept {
    return value > left && value <= right;
  }
};

}
#endif