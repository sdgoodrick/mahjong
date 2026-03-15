#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "tile.h"
#include "util.h"

template<typename T>
using TileSet = std::array<std::array<std::array<T, 32>, 17>, 5>;

template<typename T>
using Layer = std::array<std::array<T, 32>, 17>;

/// Represents the stack of tiles.
struct Board {
  Board() = default;
  void init(std::string path, std::vector<Tile>& deck);
  size_t tile_width() const {
    return 44;
  }
  size_t tile_height() const {
    return 60;
  }

  Position get_origin(Position p) const;
  std::optional<Position> find_tile(std::size_t x, std::size_t y);
  void remove_tile(const Position p);
  bool check_open(Position p) const;
  bool check_equal(const Position lhs, const Position rhs) const;
  std::size_t layers() const;
  
  // A tile is represented by a 2x2 quadrant of the tileset. A tile at (x, y) is
  // indexed in `tiles` at `tiles[layer][y / (tile_h / 2)][x / (tile_w / 2)]`.
  // `tiles[z][y][x]` is the top left corner, `tiles[z][y+1][x+1]` is
  // the bottom right corner.
  TileSet<Tile*> tiles = {};
  std::optional<Position> selected = std::nullopt;

private:
  void parse_layout(std::string path, std::vector<Tile>& deck);
};
