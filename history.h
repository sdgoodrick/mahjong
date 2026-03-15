#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "tile.h"
#include "util.h"

struct Match {
  Match() = default;
  Match(Position, Tile*, Position, Tile*);
  std::pair<Position, Tile*> a;
  std::pair<Position, Tile*> b;
};

struct History {
  std::optional<Match> rewind();
  std::optional<Match> restore();

  template<typename... Args>
  void record(Args&& ...args) {
    matches.resize(cursor++);
    matches.emplace_back(std::forward<Args>(args)...);
  }

private:
  std::vector<Match> matches;
  std::size_t cursor = 0;
};
