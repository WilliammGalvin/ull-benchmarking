#pragma once

#include "timing.hpp"
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct BatchBenchResult {
  std::string label;
  timing::Cycles64 min;
  timing::Cycles64 max;
  double avg;
  std::size_t iterations;
};

inline void print_benchmark(const BatchBenchResult &res) {
  std::cout << std::left << std::setw(35) << res.label << std::right
            << std::setw(10) << res.min << std::setw(10) << res.max
            << std::setw(10) << std::fixed << std::setprecision(1) << res.avg
            << '\n';
}

inline void print_benchmarks(const std::vector<BatchBenchResult> &results) {
  if (results.empty())
    return;

  std::string header =
      "Benchmark (" + std::to_string(results[0].iterations) + ")";
  std::cout << std::left << std::setw(35) << header << std::right
            << std::setw(10) << "Min" << std::setw(10) << "Max" << std::setw(10)
            << "Avg" << '\n';
  std::cout << std::string(70, '-') << '\n';
  for (const auto &res : results) {
    print_benchmark(res);
  }
}
