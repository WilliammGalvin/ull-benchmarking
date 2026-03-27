#pragma once

#include "timing.hpp"
#include <cstddef>
#include <vector>

struct SystemInfo {
  char cpu_model[128];
  char compiler[64];
  char build_type[16];
  std::uint32_t l1_cache_bytes;
  std::uint32_t l2_cache_bytes;
  std::uint32_t l3_cache_bytes;
  std::uint64_t timestamp;
};

struct PolicyInfo {
  char storage[32];
  char index[32];
  char overflow[32];
  char prefetch[32];
};

struct BenchConfig {
  char label[64];
  PolicyInfo policies;
  std::size_t element_size;
  std::size_t buffer_capacity;
  std::size_t num_runs;
  std::vector<timing::Cycles64> raw_cycles;
};

struct BenchSession {
  SystemInfo system;
  std::vector<BenchConfig> configs;
};
