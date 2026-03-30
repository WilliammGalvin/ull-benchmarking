#pragma once

#include "timing.hpp"
#include <cstddef>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ull {

struct SingleTestResult {
  std::optional<std::string_view> error_msg;
  [[nodiscard]] bool error() const noexcept { return error_msg.has_value(); }
};

template <timing::Timer T> struct BatchTestResult {
  std::string label;
  std::vector<typename T::Duration> results;
  std::optional<std::vector<std::string_view>> errs;

  [[nodiscard]] static inline BatchTestResult<T> empty(std::string_view label) {
    return {std::string{label}, {}, std::nullopt};
  }

  [[nodiscard]] static inline BatchTestResult<T>
  fail(std::string_view label, std::vector<typename T::Duration> &&results,
       std::vector<std::string_view> &&errs) {
    return {std::string{label}, std::move(results), std::move(errs)};
  }
};

using TestFn = std::function<SingleTestResult()>;
using PreTestFn = std::function<void()>;

struct BenchCase {
  std::string label;
  TestFn test;
  PreTestFn pre_test{[] {}};
};

template <timing::Timer T> struct Benchmark {
  using Duration = typename T::Duration;

  static constexpr int ErrorCancelCount = 10;

  [[nodiscard]] BatchTestResult<T> run(const BenchCase &bcase,
                                       std::size_t iterations = 1,
                                       std::size_t warmup = 100) const {
    const auto &[label, test, pre_test] = bcase;
    if (iterations == 0)
      return BatchTestResult<T>::empty(label);

    // Warmup - prime instruction cache, branch predictor, and TLB
    for (std::size_t i = 0; i < warmup; ++i) {
      pre_test();
      timing::do_not_optimize(bcase.test());
    }

    const T timer;
    int err_count = 0;
    std::vector<std::string_view> errs{};

    std::vector<Duration> results{};
    results.reserve(iterations);

    for (std::size_t i = 0; i < iterations; ++i) {
      pre_test();
      const Duration start = timer.start();
      SingleTestResult res = test();
      const Duration stop = timer.stop();

      timing::do_not_optimize(res);
      timing::memory_fence();

      if (res.error()) {
        ++err_count;
        errs.push_back(std::move(*res.error_msg));
      }

      results.push_back(timer.elapsed(start, stop));

      if (err_count >= ErrorCancelCount)
        return BatchTestResult<T>::fail(label, std::move(results),
                                        std::move(errs));
    }

    return {std::string{label}, std::move(results),
            errs.empty() ? std::nullopt : std::optional(std::move(errs))};
  }

  [[nodiscard]] std::vector<BatchTestResult<T>>
  run(const std::span<const BenchCase> tests, std::size_t iterations = 1,
      std::size_t warmup = 100) const {
    std::vector<BatchTestResult<T>> results;
    for (const auto &bcase : tests) {
      results.push_back(run(bcase, iterations, warmup));
    }
    return results;
  }
};

} // namespace ull
