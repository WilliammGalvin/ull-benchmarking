#pragma once

#include "bench_result.hpp"
#include <chrono>
#include <cstring>
#include <fstream>

inline SystemInfo get_system_info() {
  SystemInfo info;

  // CPU model
  std::ifstream cpuinfo("/proc/cpuinfo");
  std::string line;
  while (std::getline(cpuinfo, line)) {
    if (line.starts_with("model name")) {
      auto pos = line.find(':');
      if (pos != std::string::npos) {
        std::strncpy(info.cpu_model, line.substr(pos + 2).c_str(),
                     sizeof(info.cpu_model) - 1);
        break;
      }
    }
  }

  // Cache sizes
  auto read_cache = [](const char *path) -> std::uint32_t {
    std::ifstream f(path);
    if (!f.is_open())
      return 0;
    std::string val;
    std::getline(f, val);
    if (val.empty())
      return 0;
    if (val.ends_with("K"))
      return static_cast<std::uint32_t>(std::stoul(val) * 1024);
    if (val.ends_with("M"))
      return static_cast<std::uint32_t>(std::stoul(val) * 1024 * 1024);
    return static_cast<std::uint32_t>(std::stoul(val));
  };

  info.l1_cache_bytes =
      read_cache("/sys/devices/system/cpu/cpu0/cache/index0/size");
  info.l2_cache_bytes =
      read_cache("/sys/devices/system/cpu/cpu0/cache/index2/size");
  info.l3_cache_bytes =
      read_cache("/sys/devices/system/cpu/cpu0/cache/index3/size");

  // Compiler version
  std::strncpy(info.compiler, __VERSION__, sizeof(info.compiler) - 1);

  // Build type
#ifdef NDEBUG
  std::strncpy(info.build_type, "Release", sizeof(info.build_type) - 1);
#else
  std::strncpy(info.build_type, "Debug", sizeof(info.build_type) - 1);
#endif

  // Timestamp
  info.timestamp = static_cast<std::uint64_t>(
      std::chrono::system_clock::now().time_since_epoch().count());

  return info;
}
