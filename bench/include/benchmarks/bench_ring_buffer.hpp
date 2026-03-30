#pragma once

#include "benchmark.hpp"
#include "ring_buffer.hpp"
#include "timing.hpp"
#include <cstddef>
#include <cstring>
#include <type_traits>

namespace ull::benchmarks::ring_buffer {

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
[[nodiscard]] SingleTestResult push() noexcept {
  static_assert(std::is_default_constructible_v<T>,
                "T must be constructible for ring buffer push benchmark.");
  static_assert(std::is_copy_constructible_v<T>,
                "T must be copy constructible for ring buffer push benchmark.");

  const T val{};
  RingBuffer<T, N, Policies> rb;
  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    timing::do_not_optimize(rb.try_push(val));
  }

  return {std::nullopt};
}

} // namespace ull::benchmarks::ring_buffer
