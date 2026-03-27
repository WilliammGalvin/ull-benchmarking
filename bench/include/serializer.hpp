#pragma once

#include "bench_result.hpp"
#include <cstdint>
#include <fstream>
#include <string>

/*
 * Binary format
 * ----------------
 * [magic: 4 bytes "ULLB"]
 * [version: uint32_t]
 * [SystemInfo: fixed size]
 * [num_configs: uint64_t]
 * For each config:
 *   [label: 64 bytes]
 *   [PolicyInfo: 128 bytes]
 *   [element_size: uint64_t]
 *   [buffer_capacity: uint64_t]
 *   [num_runs: uint64_t]
 *   [raw_cycles: uint64_t * num_runs]
 */

namespace serializer {

constexpr char MAGIC[] = {'U', 'L', 'L', 'B'};
constexpr std::uint32_t VERSION = 1;

inline bool write_session(const BenchSession &session,
                          const std::string &path) {
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open())
    return false;

  // Header
  out.write(MAGIC, sizeof(MAGIC));
  out.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

  // System info (fixed size, write directly)
  out.write(reinterpret_cast<const char *>(&session.system),
            sizeof(SystemInfo));

  // Num configs
  auto num_configs = static_cast<std::uint64_t>(session.configs.size());
  out.write(reinterpret_cast<const char *>(&num_configs), sizeof(num_configs));

  // Each config
  for (const auto &config : session.configs) {
    out.write(config.label, sizeof(config.label));
    out.write(reinterpret_cast<const char *>(&config.policies),
              sizeof(PolicyInfo));

    auto element_size = static_cast<std::uint64_t>(config.element_size);
    auto buffer_capacity = static_cast<std::uint64_t>(config.buffer_capacity);
    auto num_runs = static_cast<std::uint64_t>(config.num_runs);

    out.write(reinterpret_cast<const char *>(&element_size),
              sizeof(element_size));
    out.write(reinterpret_cast<const char *>(&buffer_capacity),
              sizeof(buffer_capacity));
    out.write(reinterpret_cast<const char *>(&num_runs), sizeof(num_runs));

    // Raw cycle data, contiguous uint64_t array
    out.write(
        reinterpret_cast<const char *>(config.raw_cycles.data()),
        static_cast<std::streamsize>(num_runs * sizeof(timing::Cycles64)));
  }

  return out.good();
}

} // namespace serializer
