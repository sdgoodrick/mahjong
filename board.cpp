#include "board.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string_view>

using namespace std;

void Board::init(string path, vector<Tile>& deck) {
  parse_layout(path, deck);
}

void Board::parse_layout(string path, vector<Tile>& deck) {
  using namespace std;

  ifstream file{path};
  if (!file) {
    cerr << "failed to open layout file: " << path << endl;
    return;
  }
  
  auto x = 0;
  auto y = 0;
  auto z = 0;
  size_t max_y = 16;
  size_t cursor = 0;

  string line;
  while (getline(file, line)) {
    if (line.empty() || line.front() == '#')
      continue;

    for(char c : line) {
      if (c == '1') {
	if (cursor < deck.size()) {
	  Tile* t = &deck[cursor++];
	  tiles[z][y][x] = t;
	  tiles[z][y+1][x] = t;
	  tiles[z][y][x+1] = t;
	  tiles[z][y+1][x+1] = t;
	}
      }

      ++x;
    }

    x = 0;
    ++y;

    if (y >= max_y) {
      ++z;
      y = 0;
    }
  }
}

bool Board::check_open(Position p) const {
  assert(tiles[p.z][p.y][p.x]);
  const auto [x, y, z]  = get_origin(p);

  // Check if anything is partially covering this tile
  if (z + 1 < layers()) {
    const auto layer_above = tiles[z + 1];
    if (layer_above[y][x] ||
	layer_above[y][x + 1] ||
	layer_above[y + 1][x + 1] ||
	layer_above[y + 1][x])
      return false;
  }

  // Check if neighbors are open
  const auto& layer = tiles[p.z];
  const auto max_x = layer[0].size();
  const auto max_y = layer.size();
 
  bool left_blocked = (x > 0) &&
    (layer[y][x - 1] ||
     (y < max_y && layer[y + 1][x - 1]));

  bool right_blocked = (x + 2 < max_x) &&
    (layer[y][x + 2] ||
     (y < max_y && layer[y + 1][x + 2]));

  return !left_blocked || !right_blocked;
}

Position Board::get_origin(Position p) const {
  const auto [x, y, z] = p;
  const auto& layer = tiles[z];
  auto origin_x = x;
  auto origin_y = y;

  if (origin_x > 0 && layer[origin_y][origin_x - 1] == layer[y][x]) {
    origin_x -= 1;
  }

  if (origin_y > 0 && layer[origin_y - 1][origin_x] == layer[y][x]) {
    origin_y -= 1;
  }

  return Position{origin_x, origin_y, z};
}

Tile* Board::tile(const Position p) {
  return tiles[p.z][p.y][p.x];
}

void Board::remove_tile(const Position p) {
  const auto [x, y, z] = get_origin(p);
  tiles[z][y][x] = nullptr;
  tiles[z][y][x + 1] = nullptr;
  tiles[z][y + 1][x] = nullptr;
  tiles[z][y + 1][x + 1] = nullptr;
}

// Restore a tile
// PRECONDITION: the position must be the origin of the tile
void Board::restore_tile(Tile *t, Position origin) {
  const auto [x, y, z] = origin;
  tiles[z][y][x] = t;
  tiles[z][y][x + 1] = t;
  tiles[z][y + 1][x] = t;
  tiles[z][y + 1][x + 1] = t;
}

bool Board::check_equal(const Position lhs, const Position rhs) const {
  return lhs != rhs && *tiles[lhs.z][lhs.y][lhs.x] == *tiles[rhs.z][rhs.y][rhs.x];
}

size_t Board::tile_width() const {
    return 44;
  }

size_t Board::tile_height() const {
    return 60;
  }

size_t Board::layers() const {
  return tiles.size();
}

size_t Board::length() const {
  return tiles.front().size();
}

size_t Board::width() const {
  return tiles.front().front().size();
}
