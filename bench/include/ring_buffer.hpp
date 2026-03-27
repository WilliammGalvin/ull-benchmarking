#pragma once

#include "policies/index_policy.hpp"
#include "policies/overflow_policy.hpp"
#include "policies/prefetch_policy.hpp"
#include "policies/storage_policy.hpp"
#include <cstddef>
#include <new>
#include <type_traits>

template <typename P, typename T>
concept RingBufferPolicies = requires {
  typename P::storage;
  typename P::index;
  typename P::overflow;
  typename P::prefetch;
  requires StoragePolicy<typename P::storage, T>;
  requires IndexPolicy<typename P::index>;
  requires OverflowPolicy<typename P::overflow>;
  requires PrefetchPolicy<typename P::prefetch>;
};

template <typename T, std::size_t N,
          StoragePolicy<T> Storage = ArrayStorage<T, N>,
          IndexPolicy Index = PowerOfTwoIndex<N>,
          OverflowPolicy Overflow = RejectPolicy,
          PrefetchPolicy Prefetch = NoPrefetch>
struct DefaultRingBufferPolicies {
  using storage = Storage;
  using index = Index;
  using overflow = Overflow;
  using prefetch = Prefetch;
};

template <typename T, std::size_t N,
          RingBufferPolicies<T> Policies = DefaultRingBufferPolicies<T, N>>
class alignas(std::hardware_destructive_interference_size) RingBuffer {
  static_assert(N > 0, "Capacity must be non-zero.");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T must be nothrow move constructible.");

public:
  [[nodiscard]] bool try_push(const T &val) noexcept {
    if (full()) {
      if constexpr (Policies::overflow::should_overwrite()) {
        head_ = Policies::index::wrap(head_ + 1);
      } else {
        return false;
      }
    }

    auto slot = Policies::index::wrap(tail_);
    storage_[slot] = val;
    tail_ = Policies::index::wrap(tail_ + 1);
    Policies::prefetch::prefetch(&storage_[Policies::index::wrap(tail_)]);
    return true;
  }

  [[nodiscard]] bool try_push(T &&val) noexcept {
    if (full()) {
      if constexpr (Policies::overflow::should_overwrite()) {
        head_ = Policies::index::wrap(head_ + 1);
      } else {
        return false;
      }
    }

    auto slot = Policies::index::wrap(tail_);
    storage_[slot] = std::move(val);
    tail_ = Policies::index::wrap(tail_ + 1);
    Policies::prefetch::prefetch(&storage_[Policies::index::wrap(tail_)]);
    return true;
  }

  [[nodiscard]] bool try_pop(T &out) noexcept {
    if (empty())
      return false;

    Policies::prefetch::prefetch(&storage_[Policies::index::wrap(head_ + 1)]);
    out = std::move(storage_[Policies::index::wrap(head_)]);
    head_ = Policies::index::wrap(head_ + 1);
    return true;
  }

  [[nodiscard]] std::size_t count() const noexcept {
    return Policies::index::wrap(tail_ - head_);
  }

  [[nodiscard]] bool full() const noexcept {
    return Policies::index::wrap(tail_ + 1) == head_;
  }

  [[nodiscard]] bool empty() const noexcept { return head_ == tail_; }

  [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }

private:
  Policies::storage storage_{};
  std::size_t head_{};
  std::size_t tail_{};
};
