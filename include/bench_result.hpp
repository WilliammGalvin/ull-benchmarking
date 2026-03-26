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
            << std::setw(5) << res.iterations << '\n';
}

inline void print_benchmarks(const std::vector<BatchBenchResult> &results) {
  std::cout << std::left << std::setw(35) << "Benchmark" << std::right
            << std::setw(10) << "Min" << std::setw(10) << "Max" << std::setw(10)
            << "Avg" << std::setw(5) << "N" << '\n';
  std::cout << std::string(70, '-') << '\n';
  for (const auto &res : results) {
    print_benchmark(res);
  }
}
