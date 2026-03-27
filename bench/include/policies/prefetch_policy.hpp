#pragma once

#include <concepts>

template <typename P>
concept PrefetchPolicy = requires(const void *addr) {
  { P::prefetch(addr) } noexcept -> std::same_as<void>;
};

//
// No prefetching
//
struct NoPrefetch {
  static void prefetch(const void *) noexcept {}
};

//
// Software prefetch into L1 cache
//
struct L1Prefetch {
  static void prefetch(const void *addr) noexcept {
    __builtin_prefetch(addr, 0, 3);
  }
};

//
// Software prefetch into L2 cache
//
struct L2Prefetch {
  static void prefetch(const void *addr) noexcept {
    __builtin_prefetch(addr, 0, 2);
  }
};
