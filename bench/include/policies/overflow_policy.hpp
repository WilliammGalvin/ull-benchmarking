#pragma once

#include <concepts>

namespace ull::policies {

template <typename P>
concept OverflowPolicy = requires {
  { P::should_overwrite() } noexcept -> std::same_as<bool>;
};

//
// Reject push when full
//
struct RejectPolicy {
  static constexpr bool should_overwrite() noexcept { return false; }
};

//
// Overwrite oldest entry when full
//
struct OverwritePolicy {
  static constexpr bool should_overwrite() noexcept { return true; }
};

} // namespace ull::policies
