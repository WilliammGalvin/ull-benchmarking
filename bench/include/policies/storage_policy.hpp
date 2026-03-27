#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <deque>
#include <memory>

template <typename S, typename T>
concept StoragePolicy =
    requires(S storage, const S const_storage, std::size_t idx) {
      { storage[idx] } noexcept -> std::same_as<T &>;
      { const_storage[idx] } noexcept -> std::same_as<const T &>;
    };

//
// Contiguous, cache-friendly storage
//
template <typename T, std::size_t N> struct ArrayStorage {
  std::array<T, N> data_{};

  T &operator[](std::size_t idx) noexcept { return data_[idx]; }
  const T &operator[](std::size_t idx) const noexcept { return data_[idx]; }
};

//
// Pointer chasing, cache-hostile storage
//
template <typename T, std::size_t N> struct LinkedStorage {
  struct Node {
    T value_{};
    std::size_t next_{};
  };

  std::array<std::unique_ptr<Node>, N> nodes_{};

  LinkedStorage() {
    for (std::size_t i = 0; i < N; ++i) {
      nodes_[i] = std::make_unique<Node>();
      nodes_[i]->next_ = (i + 1) % N;
    }
  }

  T &operator[](std::size_t idx) noexcept { return nodes_[idx]->value_; }
  const T &operator[](std::size_t idx) const noexcept {
    return nodes_[idx]->value_;
  }
};

//
// Chunked storage, partial locality
//
template <typename T, std::size_t N> struct DequeStorage {
  std::deque<T> data_;

  DequeStorage() : data_(N) {}

  T &operator[](std::size_t idx) noexcept { return data_[idx]; }
  const T &operator[](std::size_t idx) const noexcept { return data_[idx]; }
};
