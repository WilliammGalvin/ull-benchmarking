#include "bench_result.hpp"
#include "benchmarks/bench_ring_buffer.hpp"
#include <vector>

int main() {
  std::vector<BatchBenchResult> results;
  run_all_benchmarks_ring_buffer<int, 1024, 1000>(results);
  print_benchmarks(results);
  return 0;
}
