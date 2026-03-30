#pragma once

#include "timing.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace ull {

struct BenchConfig {
  std::string label;
  std::size_t element_size;
  std::size_t buffer_capacity;
  std::size_t num_runs;
  std::vector<timing::Cycles64> raw_cycles;
};

struct BenchSession {
  std::vector<BenchConfig> configs;
};

} // namespace ull
