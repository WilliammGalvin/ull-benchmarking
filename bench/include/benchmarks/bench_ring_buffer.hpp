#pragma once

#include "bench_result.hpp"
#include "ring_buffer.hpp"
#include "timing.hpp"
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <vector>

namespace {
template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
[[nodiscard]] timing::Cycles64 bench_push_test() noexcept {
  static_assert(std::is_default_constructible_v<T>,
                "T must be constructible for ring buffer push benchmark.");
  static_assert(std::is_copy_constructible_v<T>,
                "T must be copy constructible for ring buffer push benchmark.");

  const T val{};
  RingBuffer<T, N, Policies> rb;
  const auto start = timing::start();

  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    (void)rb.try_push(val);
  }

  const auto stop = timing::stop();
  return timing::cycles(start, stop);
}
} // namespace

template <typename Data_T, std::size_t Buffer_N, std::size_t Tests_N,
          RingBufferPolicies<Data_T> Policies =
              DefaultRingBufferPolicies<Data_T, Buffer_N>>
[[nodiscard]] BenchConfig
bench_ring_buffer(const char *label, const char *storage, const char *index,
                  const char *overflow, const char *prefetch) {
  BenchConfig config{};
  std::strncpy(config.label, label, sizeof(config.label) - 1);
  std::strncpy(config.policies.storage, storage,
               sizeof(config.policies.storage) - 1);
  std::strncpy(config.policies.index, index, sizeof(config.policies.index) - 1);
  std::strncpy(config.policies.overflow, overflow,
               sizeof(config.policies.overflow) - 1);
  std::strncpy(config.policies.prefetch, prefetch,
               sizeof(config.policies.prefetch) - 1);
  config.element_size = sizeof(Data_T);
  config.buffer_capacity = Buffer_N;
  config.num_runs = Tests_N;
  config.raw_cycles.reserve(Tests_N);

  for (std::size_t i = 0; i < Tests_N; ++i) {
    config.raw_cycles.push_back(bench_push_test<Data_T, Buffer_N, Policies>());
  }

  return config;
}

template <typename T, std::size_t Buffer_N, std::size_t Tests_N>
void run_all_benchmarks_ring_buffer(BenchSession &session) {
  using Fast = DefaultRingBufferPolicies<T, Buffer_N>;
  using Modulo =
      DefaultRingBufferPolicies<T, Buffer_N, ArrayStorage<T, Buffer_N>,
                                ModuloIndex<Buffer_N>>;
  using Linked =
      DefaultRingBufferPolicies<T, Buffer_N, LinkedStorage<T, Buffer_N>>;
  using Prefetched =
      DefaultRingBufferPolicies<T, Buffer_N, ArrayStorage<T, Buffer_N>,
                                PowerOfTwoIndex<Buffer_N>, RejectPolicy,
                                L1Prefetch>;

  session.configs.push_back(bench_ring_buffer<T, Buffer_N, Tests_N, Fast>(
      "array + bitmask", "ArrayStorage", "PowerOfTwoIndex", "RejectPolicy",
      "NoPrefetch"));
  session.configs.push_back(bench_ring_buffer<T, Buffer_N, Tests_N, Modulo>(
      "array + modulo", "ArrayStorage", "ModuloIndex", "RejectPolicy",
      "NoPrefetch"));
  session.configs.push_back(bench_ring_buffer<T, Buffer_N, Tests_N, Linked>(
      "linked + bitmask", "LinkedStorage", "PowerOfTwoIndex", "RejectPolicy",
      "NoPrefetch"));
  session.configs.push_back(bench_ring_buffer<T, Buffer_N, Tests_N, Prefetched>(
      "array + bitmask + L1 prefetch", "ArrayStorage", "PowerOfTwoIndex",
      "RejectPolicy", "L1Prefetch"));
}
