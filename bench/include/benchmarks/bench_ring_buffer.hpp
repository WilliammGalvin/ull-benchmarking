#pragma once

#include "benchmark.hpp"
#include "ring_buffer.hpp"
#include "timing/timing.hpp"
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

namespace ull::benchmarks::ring_buffer {

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
SingleTestResult push() noexcept {
  static_assert(std::is_default_constructible_v<T>);
  static_assert(std::is_copy_constructible_v<T>);

  const T val{};
  RingBuffer<T, N, Policies> rb;
  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    timing::do_not_optimize(rb.try_push(val));
  }

  return {std::nullopt};
}

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
SingleTestResult pop(RingBuffer<T, N, Policies> &rb) noexcept {
  static_assert(std::is_default_constructible_v<T>);

  T out{};
  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    timing::do_not_optimize(rb.try_pop(out));
  }

  return {std::nullopt};
}

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
SingleTestResult push_pop() noexcept {
  static_assert(std::is_default_constructible_v<T>);

  const T val{};
  T out{};
  RingBuffer<T, N, Policies> rb;
  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    timing::do_not_optimize(rb.try_push(val));
    timing::do_not_optimize(rb.try_pop(out));
  }

  return {std::nullopt};
}

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
SingleTestResult fill_then_drain(RingBuffer<T, N, Policies> &rb) noexcept {
  T out{};
  for (std::size_t i = 0; i < rb.capacity(); ++i) {
    timing::do_not_optimize(rb.try_pop(out));
  }

  return {std::nullopt};
}

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
void prefill(RingBuffer<T, N, Policies> &rb) noexcept {
  const T val{};

  T out{};
  while (!rb.empty())
    (void)rb.try_pop(out);

  for (std::size_t i = 0; i < rb.capacity(); ++i)
    (void)rb.try_push(val);
}

template <typename T, std::size_t N, RingBufferPolicies<T> Policies>
std::vector<BenchCase> make_ring_buffer_suite(std::string_view tag) {
  auto rb = std::make_shared<RingBuffer<T, N, Policies>>();

  return {
      {std::string(tag) + " | push",
       benchmarks::ring_buffer::push<T, N, Policies>},

      {std::string(tag) + " | pop",
       [rb]() { return benchmarks::ring_buffer::pop(*rb); },
       [rb]() { benchmarks::ring_buffer::prefill(*rb); }},

      {std::string(tag) + " | push_pop",
       benchmarks::ring_buffer::push_pop<T, N, Policies>},

      {std::string(tag) + " | fill_then_drain",
       [rb]() { return benchmarks::ring_buffer::fill_then_drain(*rb); },
       [rb]() { benchmarks::ring_buffer::prefill(*rb); }},
  };
}
} // namespace ull::benchmarks::ring_buffer
