#include "bench_result.hpp"
#include "benchmarks/bench_ring_buffer.hpp"
#include <vector>

int main() {
  std::cout << "\n";

  std::vector<BatchBenchResult> results;
  run_all_benchmarks_ring_buffer<int, 1024, 1000>(results);
  print_benchmarks(results);

  std::cout << "\n";

  results.clear();
  run_all_benchmarks_ring_buffer<int, 1024, 10'000>(results);
  print_benchmarks(results);

  std::cout << "\n";

  results.clear();
  run_all_benchmarks_ring_buffer<int, 1024, 1'000'000>(results);
  print_benchmarks(results);

  std::cout << "\n";

  return 0;
}
