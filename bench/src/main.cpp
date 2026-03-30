#include "benchmark.hpp"
#include "benchmarks/bench_ring_buffer.hpp"
#include "sys/cpu.hpp"
#include <cstdio>
#include <vector>

int main() {
  using namespace ull;
  using Timer = timing::BenchmarkTimer<timing::TimingPolicy::CPU>;
  using Bench = Benchmark<Timer>;
  using Duration = typename Timer::Duration;

  // Fallback, prefer taskset -c 0 via 'make run-pinned'
  if (!sys::pin_to_core(0)) {
    std::fprintf(stderr, "Failed to pin CPU.\n");
    return 1;
  }

  sys::check_frequency_governor();

  using Bitmask = DefaultRingBufferPolicies<int, 1024>;
  using Modulo = DefaultRingBufferPolicies<int, 1024, ArrayStorage<int, 1024>,
                                           ModuloIndex<1024>>;

  auto bitmask_tests =
      benchmarks::ring_buffer::make_ring_buffer_suite<int, 1024, Bitmask>(
          "array + bitmask");
  auto modulo_tests =
      benchmarks::ring_buffer::make_ring_buffer_suite<int, 1024, Modulo>(
          "array + modulo");

  std::vector<BenchCase> all;
  all.insert(all.end(), std::make_move_iterator(bitmask_tests.begin()),
             std::make_move_iterator(bitmask_tests.end()));
  all.insert(all.end(), std::make_move_iterator(modulo_tests.begin()),
             std::make_move_iterator(modulo_tests.end()));

  Bench bench;
  const auto results = bench.run(all, 10'000);

  for (const auto &result : results) {
    Duration total = 0;
    for (auto sample : result.results)
      total += sample;

    Duration mean = total / result.results.size();
    std::printf("%-40s | mean: %lu cycles | samples: %zu\n",
                std::string(result.label).c_str(), mean, result.results.size());
  }

  return 0;
}
