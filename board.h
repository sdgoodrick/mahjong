#include <array>
#include <string_view>
#include <iostream>
#include <fstream>

#include "tile.h"

template<typename T>
using TileSet = std::array<std::array<std::array<T, 32>, 17>, 5>;

template<typename T>
using Layer = std::array<std::array<T, 32>, 17>;

/// Represents the stack of tiles.
struct Board {
  Board(std::string path, std::vector<Tile>& deck);
  size_t tile_width() const {
    return 44;
  }
  size_t tile_height() const {
    return 60;
  }
  
  // A tile is represented by a 2x2 quadrant of the tileset. A tile at (x, y) is
  // indexed in `tiles` at `tiles[layer][y / (tile_h / 2)][x / (tile_w / 2)]`.
  // `tiles[z][y][x]` is the top left corner, `tiles[z][y+1][x+1]` is
  // the bottom right corner.
  TileSet<Tile*> tiles = {};
  Tile* selected_tile = nullptr;

private:
  void parse_layout(std::string path, std::vector<Tile>& deck);
};

static constexpr std::string_view turtle_layer_0 = R"(
...121212121212121212121212.....
...434343434343434343434343.....
.......1212121212121212.........
.......4343434343434343.........
.....12121212121212121212.......
.....43434343434343434343.......
...121212121212121212121212.....
.124343434343434343434343431212.
.431212121212121212121212124343.
...434343434343434343434343.....
.....12121212121212121212.......
.....43434343434343434343.......
.......1212121212121212.........
.......4343434343434343.........
...121212121212121212121212.....
...434343434343434343434343.....
)";

Board::Board(std::string path, std::vector<Tile>& deck) {
  parse_layout(path, deck);
  /* auto x = 0; */
  /* auto y = 0; */
  /* auto z = 0; */
  /* std::size_t cursor = 0; */

  /* for(char c : turtle_layer_0) { */
  /*   if (c == '\n') { */
  /*     x = 0; */
  /*     ++y; */
  /*     continue; */
  /*   } */

  /*   if (c == '1') { */
  /*     if (cursor < deck.size()) { */
  /* 	Tile* t = &deck[cursor++]; */
  /* 	tiles[z][y][x] = t; */
  /* 	tiles[z][y+1][x] = t; */
  /* 	tiles[z][y][x+1] = t; */
  /* 	tiles[z][y+1][x+1] = t; */
  /*     } */
  /*   } */

  /*   ++x; */
  /* } */
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
