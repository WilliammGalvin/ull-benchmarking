#pragma once

#include <cstdint>

namespace timing {
using Cycles32 = std::uint32_t;
using Cycles64 = std::uint64_t;

__attribute__((always_inline)) inline Cycles64 start() noexcept {
  Cycles32 lo, hi;
  asm volatile("cpuid\n"
               "rdtsc"
               : "=a"(lo), "=d"(hi)
               :
               : "rbx", "rcx");

  return (static_cast<Cycles64>(hi) << 32) | lo;
}

__attribute__((always_inline)) inline Cycles64 stop() noexcept {
  Cycles32 lo, hi;
  asm volatile("rdtscp\n"
               "mov %%eax, %0\n"
               "mov %%edx, %1\n"
               "cpuid"
               : "=r"(lo), "=r"(hi)
               :
               : "rax", "rbx", "rcx", "rdx");

  return (static_cast<Cycles64>(hi) << 32) | lo;
}

__attribute__((always_inline)) inline Cycles64 cycles(Cycles64 begin,
                                                      Cycles64 end) noexcept {
  return end - begin;
}

struct ScopeTimer {
  Cycles64 &out_;
  Cycles64 begin_;

  explicit ScopeTimer(Cycles64 &out) noexcept : out_(out), begin_(start()) {}

  ~ScopeTimer() noexcept { out_ = cycles(begin_, stop()); }

  ScopeTimer(const ScopeTimer &) = delete;
  ScopeTimer &operator=(const ScopeTimer &) = delete;
};

} // namespace timing
