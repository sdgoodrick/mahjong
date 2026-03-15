#pragma once

#include <tuple>

template<typename T>
using Point = std::tuple<T, T>;

struct Position {
  std::size_t x;
  std::size_t y;
  std::size_t z;

  bool operator==(const Position& rhs) const = default;
  auto operator<=>(const Position& rhs) const = default;
};
