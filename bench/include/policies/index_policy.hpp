#pragma once

#include <concepts>
#include <cstddef>

namespace ull::policies {

template <typename P>
concept IndexPolicy = requires(std::size_t idx) {
  { P::wrap(idx) } noexcept -> std::same_as<std::size_t>;
};

//
// Bitmask index, requires power of two capacity
//
template <std::size_t N> struct PowerOfTwoIndex {
  static_assert((N & (N - 1)) == 0,
                "PowerOfTwoIndex requires power-of-2 capacity.");
  static_assert(N > 0, "Capacity must be non-zero.");

  static constexpr std::size_t wrap(std::size_t idx) noexcept {
    return idx & (N - 1);
  }
};

//
// Modulo index, works with any capacity
//
template <std::size_t N> struct ModuloIndex {
  static_assert(N > 0, "Capacity must be non-zero.");

  static constexpr std::size_t wrap(std::size_t idx) noexcept {
    return idx % N;
  }
};

} // namespace ull::policies
