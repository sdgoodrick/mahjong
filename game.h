#pragma once

#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include "board.h"
#include "graphics.h"
#include "tile.h"
#include "util.h"

using Deck = std::vector<Tile>;

template<typename T>
using Point = std::tuple<T, T>;

using Match = std::pair<std::pair<Position, Tile*>, std::pair<Position, Tile*>>; 

struct History {
  std::vector<Match> matches;
  std::size_t cursor = 0;
};

struct Game {
  std::optional<Position> selected_tile;

  Game();
  void draw_tiles(Graphics& g);
  void draw_selection(Graphics &g);
  void handle_click(Graphics& g, Point<float> click);
  void handle_undo();
  void handle_redo();
private:
  void create_deck();

  Board board;
  Deck deck;
  History history;
};
