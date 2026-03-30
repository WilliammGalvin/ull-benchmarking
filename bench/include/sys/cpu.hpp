#pragma once

#include <cstdio>
#include <fstream>
#include <sched.h>

namespace ull::sys {

// Pins the calling thread to a single core
inline bool pin_to_core(unsigned core = 0) noexcept {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core, &mask);

  if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
    std::perror("sched_setaffinity");
    return false;
  }

  return true;
}

// Warns if CPU governor isn't set to performance mode
inline bool check_frequency_governor() noexcept {
  std::ifstream gov("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
  std::string current;
  if (gov >> current) {
    if (current != "performance") {
      std::fprintf(stderr,
                   "Warning: CPU governor is '%s', not 'performance'. "
                   "Results may be inconsistent.\n",
                   current.c_str());

      return false;
    }
  }

  return true;
}
} // namespace ull::sys
