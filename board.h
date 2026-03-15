#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "tile.h"
#include "util.h"

template<typename T>
using TileSet = std::array<std::array<std::array<T, 32>, 17>, 5>;

/// Represents the stack of tiles.
struct Board {
  Board() = default;
  void init(std::string path, std::vector<Tile>& deck);

  Tile* tile(const Position p);

  Position get_origin(Position p) const;
  std::optional<Position> find_tile(std::size_t x, std::size_t y);
  void remove_tile(const Position p);
  void restore_tile(Tile *t, Position origin);

  bool check_open(Position p) const;
  bool check_equal(const Position lhs, const Position rhs) const;

  // Logical information
  size_t tile_width() const;
  size_t tile_height() const;
  std::size_t layers() const;
  std::size_t length() const;
  std::size_t width() const;
  
  // A tile is represented by a 2x2 quadrant of the tileset. A tile at (x, y) is
  // indexed in `tiles` at `tiles[layer][y / (tile_h / 2)][x / (tile_w / 2)]`.
  // `tiles[z][y][x]` is the top left corner, `tiles[z][y+1][x+1]` is
  // the bottom right corner.
  std::optional<Position> selected = std::nullopt;

private:
  TileSet<Tile*> tiles = {};
  void parse_layout(std::string path, std::vector<Tile>& deck);
};
