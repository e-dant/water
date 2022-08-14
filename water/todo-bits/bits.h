#pragma once
namespace bits {
  /** @brief get
   *  Returns the range of bits for the inclusive
   *  range of bits between `begin` and `end` in
   *  the given `bits`.
   *  This function provides ample bounds checking
   *  and handles all the edge cases Will could
   *  think of.
   *  The `#if` macro here is checking if C++17
   *  features are supported.
   **/
  #if __cplusplus >= 201703L
  inline static constexpr const unsigned get(const auto start,
                                             const auto end,
                                             auto bits) noexcept
  #else
  inline unsigned get(const unsigned start,
                      const unsigned end,
                      unsigned bits) noexcept
  #endif
  {
    // greatest value this type can hold
    static constexpr const unsigned max_n =
        std::numeric_limits<decltype(max_n)>::max();
    // return 0 if 'start' is left-padded
    // with infinite zeros
    if_unlikely(start >= max_n) return 0;
    // drop bits before 'start'
    // bits >>= start;
    // if gt/eq here, then return all the bits,
    // shifted right by start.
    // the result won't be accurate but undefined
    // behavior will be even less accurate.
    else if_unlikely((end - start) >= max_n) return bits >> start;
    // if none of the above edge-cases, then
    // continue with the usual shift-and-mask
    const unsigned mask = (1u << (end - start)) - 1;  // n '1's
    return (bits >> start) & mask;                    // chop and return
  }
  }  // namespace bits
