#include "board.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string_view>

void Board::init(std::string path, std::vector<Tile>& deck) {
  parse_layout(path, deck);
}

void Board::parse_layout(std::string path, std::vector<Tile>& deck) {
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
  auto origin = get_origin(p);

  // Check if anything is partially covering this tile
  if (origin.z + 1 < layers()) {
    const auto layer_above = tiles[origin.z + 1];
    if (layer_above[origin.y][origin.x] ||
	layer_above[origin.y][origin.x + 1] ||
	layer_above[origin.y + 1][origin.x + 1] ||
	layer_above[origin.y + 1][origin.x])
      return false;
  }

  // Check if neighbors are open
  auto layer = tiles[p.z];
  const auto max_x = layer[0].size();
  const auto max_y = layer.size();
 
  bool left_blocked = (origin.x > 0) &&
    (layer[origin.y][origin.x - 1] ||
     (origin.y < max_y && layer[origin.y + 1][origin.x - 1]));

  bool right_blocked = (origin.x + 2 < max_x) &&
    (layer[origin.y][origin.x + 2] ||
     (origin.y < max_y && layer[origin.y + 1][origin.x + 2]));

  return !left_blocked || !right_blocked;
}

Position Board::get_origin(Position p) const {
  auto origin_x = p.x;
  auto origin_y = p.y;
  auto layer = tiles[p.z];

  if (origin_x > 0 && layer[origin_y][origin_x - 1] == layer[p.y][p.x]) {
    origin_x -= 1;
  }

  if (origin_y > 0 && layer[origin_y - 1][origin_x] == layer[p.y][p.x]) {
    origin_y -= 1;
  }

  return Position{origin_x, origin_y, p.z};
}

void Board::remove_tile(const Position p) {
  auto origin = get_origin(p);
  tiles[origin.z][origin.y][origin.x] = nullptr;
  tiles[origin.z][origin.y][origin.x + 1] = nullptr;
  tiles[origin.z][origin.y + 1][origin.x] = nullptr;
  tiles[origin.z][origin.y + 1][origin.x + 1] = nullptr;
}

void Board::restore_tile(Tile *t, Position origin) {
  tiles[origin.z][origin.y][origin.x] = t;
  tiles[origin.z][origin.y][origin.x + 1] = t;
  tiles[origin.z][origin.y + 1][origin.x] = t;
  tiles[origin.z][origin.y + 1][origin.x + 1] = t;
}

bool Board::check_equal(const Position lhs, const Position rhs) const {
  return lhs != rhs && *tiles[lhs.z][lhs.y][lhs.x] == *tiles[rhs.z][rhs.y][rhs.x];
}

std::size_t Board::layers() const {
  return tiles.size();
}
