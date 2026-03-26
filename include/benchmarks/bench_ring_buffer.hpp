#pragma once

#include "bench_result.hpp"
#include "ring_buffer.hpp"
#include "timing.hpp"
#include <cstddef>
#include <limits>
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
[[nodiscard]] BatchBenchResult bench_ring_buffer(const std::string &label) {
  std::vector<timing::Cycles64> results;
  results.reserve(Tests_N);

  for (std::size_t i = 0; i < Tests_N; ++i) {
    results.push_back(bench_push_test<Data_T, Buffer_N, Policies>());
  }

  timing::Cycles64 min{std::numeric_limits<timing::Cycles64>::max()};
  timing::Cycles64 max{}, sum{};
  for (const auto &res : results) {
    min = std::min(min, res);
    max = std::max(max, res);
    sum += res;
  }

  const double avg = static_cast<double>(sum) / Tests_N;
  return {label, min, max, avg, Tests_N};
}

template <typename T, std::size_t Buffer_N, std::size_t Tests_N>
void run_all_benchmarks_ring_buffer(std::vector<BatchBenchResult> &out) {
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

  out.push_back(
      bench_ring_buffer<T, Buffer_N, Tests_N, Fast>("array + bitmask"));
  out.push_back(
      bench_ring_buffer<T, Buffer_N, Tests_N, Modulo>("array + modulo"));
  out.push_back(
      bench_ring_buffer<T, Buffer_N, Tests_N, Linked>("linked + bitmask"));
  out.push_back(bench_ring_buffer<T, Buffer_N, Tests_N, Prefetched>(
      "array + bitmask + L1 prefetch"));
}
