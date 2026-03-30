#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace ull {

struct Order {
  std::uint64_t id;
  std::int64_t price;
  std::uint64_t qty;
  enum class Side : std::uint8_t { Buy, Sell } side;
};

struct PriceLevel {
  std::int64_t price;
  std::uint64_t total_qty;
  std::vector<Order> orders;
};

class OrderBook {
public:
  bool add(const Order &order);
  bool cancel(std::uint64_t order_id);
  std::optional<PriceLevel> best_bid() const;
  std::optional<PriceLevel> best_ask() const;

private:
  std::map<std::int64_t, PriceLevel, std::greater<>> bids_;
  std::map<std::int64_t, PriceLevel, std::less<>> asks_;
  std::unordered_map<std::uint64_t, std::int64_t> order_to_price_;
};

} // namespace ull
