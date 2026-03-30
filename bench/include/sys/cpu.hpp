#pragma once

#include <cstdio>
#include <sched.h>

namespace ull::sys {

// Pins the calling thread to a single core
inline bool pin_to_core(int core = 0) noexcept {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core, &mask);

  if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
    std::perror("sched_setaffinity");
    return false;
  }

  return true;
}

} // namespace ull::sys
