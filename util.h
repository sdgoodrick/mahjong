#pragma once

struct Position {
  std::size_t x;
  std::size_t y;
  std::size_t z;

  bool operator==(const Position& rhs) const = default;
};
