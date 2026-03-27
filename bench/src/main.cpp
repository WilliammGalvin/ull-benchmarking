#include "bench_result.hpp"
#include "benchmarks/bench_ring_buffer.hpp"
#include "serializer.hpp"
#include "system_info.hpp"
#include <iostream>

int main() {
  BenchSession session;
  session.system = get_system_info();
  run_all_benchmarks_ring_buffer<int, 1024, 10'000>(session);

  if (!serializer::write_session(session, "../data/bench_results.ullb")) {
    std::cerr << "Failed to write benchmark results.\n";
    return 1;
  }

  return 0;
}
