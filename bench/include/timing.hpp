#pragma once

#include <chrono>
#include <concepts>
#include <cstdint>

namespace ull::timing {
using Cycles32 = std::uint32_t;
using Cycles64 = std::uint64_t;
using Nanoseconds = std::int64_t;

namespace {
template <typename T>
concept Subtractable = requires(T a, T b) {
  { a - b } noexcept -> std::same_as<T>;
};
} // namespace

template <typename T>
concept Timer = requires(T t) {
  typename T::Duration;
  requires Subtractable<typename T::Duration>;

  { t.start() } noexcept -> std::same_as<typename T::Duration>;
  { t.stop() } noexcept -> std::same_as<typename T::Duration>;
  {
    t.elapsed(t.start(), t.stop())
  } noexcept -> std::same_as<typename T::Duration>;
};

enum class TimingPolicy { CPU, SysTime };
template <TimingPolicy> struct BenchmarkTimer;

template <typename Derived> struct TimerBase {
  __attribute__((always_inline)) [[nodiscard]] auto
  elapsed(auto start, auto stop) const noexcept {
    return stop - start;
  }
};

// Prevents the compiler from eliminating dead code in benchmark tests
template <typename T>
__attribute__((always_inline)) inline void do_not_optimize(T &&val) noexcept {
  asm volatile("" : : "g"(val) : "memory");
}

// Prevents the compiler from reordering memory operations across this barrier
__attribute__((always_inline)) inline void memory_fence() noexcept {
  asm volatile("" ::: "memory");
}

//
// CPU clock cycle count from CPU. Minimal overhead
//
template <>
struct BenchmarkTimer<TimingPolicy::CPU>
    : TimerBase<BenchmarkTimer<TimingPolicy::CPU>> {
  using Duration = Cycles64;

  __attribute__((always_inline)) inline Duration start() const noexcept {
    Cycles32 lo, hi;
    asm volatile("cpuid\n"
                 "rdtsc"
                 : "=a"(lo), "=d"(hi)
                 :
                 : "rbx", "rcx");

    return (static_cast<Cycles64>(hi) << 32) | lo;
  }

  __attribute__((always_inline)) inline Duration stop() const noexcept {
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
};

//
// System time, high overhead. Uses sys call
//
template <>
struct BenchmarkTimer<TimingPolicy::SysTime>
    : TimerBase<BenchmarkTimer<TimingPolicy::SysTime>> {
  using Duration = Nanoseconds;

  __attribute__((always_inline)) inline Duration start() const noexcept {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())
        .count();
  }

  __attribute__((always_inline)) inline Duration stop() const noexcept {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())
        .count();
  }
};

static_assert(Timer<BenchmarkTimer<TimingPolicy::CPU>>);
static_assert(Timer<BenchmarkTimer<TimingPolicy::SysTime>>);

} // namespace ull::timing
