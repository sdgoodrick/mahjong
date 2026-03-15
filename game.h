#pragma once

#include <optional>
#include <vector>

#include "board.h"
#include "graphics.h"
#include "history.h"
#include "tile.h"
#include "util.h"

using Deck = std::vector<Tile>;

struct Game {
  std::optional<Position> selected_tile;

  Game(Graphics& g);
  void draw_tiles();
  void draw_selection();
  void handle_click(Point<float> click);
  void handle_undo();
  void handle_redo();
private:
  void create_deck();

  Board board;
  Deck deck;
  History history;
  Graphics& graphics;
};
