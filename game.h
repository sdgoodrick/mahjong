#pragma once

#include <optional>
#include <tuple>
#include <vector>

#include "board.h"
#include "graphics.h"
#include "tile.h"
#include "util.h"

using Deck = std::vector<Tile>;

template<typename T>
using Point = std::tuple<T, T>;

struct Game {
  std::optional<Position> selected_tile;

  Game();
  void draw_tiles(Graphics& g);
  void draw_selection(Graphics &g);
  void handle_click(Graphics& g, Point<float> click);
private:
  void create_deck();

  Board board;
  Deck deck;
};
