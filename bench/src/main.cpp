#include "benchmark.hpp"
#include "benchmarks/bench_ring_buffer.hpp"
#include "sys/cpu.hpp"
#include <cstdio>
#include <vector>

int main() {
  using namespace ull;
  using Timer = timing::BenchmarkTimer<timing::TimingPolicy::CPU>;
  using Duration = typename Timer::Duration;

  // Fallback, prefer taskset -c 0 via 'make run-pinned'
  if (!sys::pin_to_core(0)) {
    std::fprintf(stderr, "Failed to pin CPU.\n");
    return 1;
  }

  const Benchmark<Timer> bench;
  const std::vector<typename Benchmark<Timer>::BenchCase> tests = {
      {"array + bitmask",
       benchmarks::ring_buffer::push<int, 1024,
                                     DefaultRingBufferPolicies<int, 1024>>},

      {"array + modulo",
       benchmarks::ring_buffer::push<
           int, 1024,
           DefaultRingBufferPolicies<int, 1024, ArrayStorage<int, 1024>,
                                     ModuloIndex<1024>>>},
  };

  const auto results = bench.run(tests, 10'000);

  // Display results
  for (const auto &result : results) {
    Duration total = 0;
    for (auto sample : result.results)
      total += sample;

    Duration mean = total / result.results.size();
    std::printf("%-30s | mean: %lu cycles | samples: %zu\n",
                std::string(result.label).c_str(), mean, result.results.size());
  }

  return 0;
}
