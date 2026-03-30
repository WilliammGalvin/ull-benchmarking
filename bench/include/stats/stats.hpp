#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <numeric>
#include <span>

namespace ull::stats {

namespace {
template <typename T>
concept Summarizable = requires(T a, T b) {
  { a + b } -> std::same_as<T>;
  { a - b } -> std::same_as<T>;
  { a * b } -> std::same_as<T>;
  { a / b } -> std::same_as<T>;
  { a < b } -> std::convertible_to<bool>;
};
} // namespace

template <Summarizable T> struct Summary {
  T min;
  T max;
  T mean;
  T median;
  T p90;
  T p95;
  T p99;
  T p999;
  T stddev;
  std::size_t count;
  std::size_t outliers;
};

template <Summarizable T>
[[nodiscard]] inline Summary<T>
generate_summary(const std::span<T> &samples) noexcept {
  std::sort(samples.begin(), samples.end());

  const auto n = samples.size();
  const T sum = std::accumulate(samples.begin(), samples.end(), T{0});
  const T mean = sum / static_cast<T>(n);

  T var{0};
  for (const auto &s : samples)
    var += (s - mean) * (s - mean);
  const T stddev = static_cast<T>(
      std::sqrt(static_cast<double>(var) / static_cast<double>(n)));

  const T q1 = samples[n / 4];
  const T q3 = samples[3 * n / 4];
  const T iqr = q3 - q1;
  const T lower = q1 - 3 * iqr / 2;
  const T upper = q3 + 3 * iqr / 2;

  return {
      .min = samples.front(),
      .max = samples.back(),
      .mean = mean,
      .median = samples[n / 2],
      .p90 = samples[static_cast<std::size_t>(n * 0.90)],
      .p95 = samples[static_cast<std::size_t>(n * 0.95)],
      .p99 = samples[static_cast<std::size_t>(n * 0.99)],
      .p999 = samples[static_cast<std::size_t>(n * 0.999)],
      .stddev = stddev,
      .count = n,
      .outliers = static_cast<std::size_t>(std::count_if(
          samples.begin(), samples.end(),
          [lower, upper](T v) { return v < lower || v > upper; })),
  };
}

} // namespace ull::stats
