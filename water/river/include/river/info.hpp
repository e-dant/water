#pragma once

#include <iostream>

namespace info {
template <typename Item>
inline static constexpr auto soft(const Item& item) noexcept {
  std::cerr << item << std::endl;
}
template <typename Item, typename... Items>
inline static constexpr auto soft(const Item& item,
                                 const Items&... items) noexcept {
  std::cerr << item;
  soft(items...);
}
template <typename Item>
inline static constexpr auto fatal(const Item& item) noexcept {
  std::cerr << item << "\n  ";
  std::exit(1);
}
template <typename Item, typename... Items>
inline static constexpr auto fatal(const Item& item,
                                  const Items&... items) noexcept {
  std::cerr << "\n" << item << "\n  ";
  fatal(items...);
}
}  // namespace info