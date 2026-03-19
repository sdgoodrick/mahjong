#pragma once

#include <optional>
#include <vector>

#include "board.h"
#include "graphics.h"
#include "history.h"
#include "tile.h"
#include "util.h"

using Deck = std::vector<Tile>;

enum class State {
  Attract,
  Main,
  Settings,
};

struct Game {
  std::optional<Position> selected_tile;

  Game(Graphics& g);

  // Attract state
  void draw_button();

  // Main state
  void draw_tiles();
  void draw_hud();
  void draw_selection();
  void draw_hint();
  void handle_click(Point<float> click);
  void handle_undo();
  void handle_redo();
  void handle_hint();
  void set_time(uint64_t t);

  std::size_t count_matches();
  std::size_t count_tiles();
private:
  void create_deck();
  void update_matches();
  std::optional<Match> get_hint();
  std::optional<Match> current_hint;

  Board board;
  Deck deck;
  History history;
  Graphics& graphics;

  std::vector<Match> available_matches;
  std::size_t hint_cursor = 0;

  uint64_t current_time = 0;
  uint64_t hint_start_time = 0;
};
