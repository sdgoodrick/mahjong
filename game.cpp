#include "game.h"

#include <algorithm>
#include <random>

using namespace std;

Game::Game(Graphics& graphics)
  : graphics(graphics)
{
  create_deck();

  random_device rd;
  mt19937 g(rd());
  ranges::shuffle(deck, g);
  board.init("../layouts/turtle", deck);
}

void Game::draw_tiles() {
  for (size_t z = 0; z < board.layers(); ++z) {
    for (size_t y = 0; y < board.length(); ++y) {
      for (size_t x = 0; x < board.width(); ++x) {
	Position current{x, y, z};
	Tile* t = board.tile(current);
	if (t == nullptr)
	  continue;

	const auto origin = board.get_origin(current);
	if (origin.x == current.x && origin.y == current.y)
	  graphics.draw_tile(*t, current);
      }
    }
  }
}

void Game::draw_selection() {
  if (board.selected)
    graphics.draw_selection(*board.selected);
}

void Game::create_deck() {
  deck.reserve(144);

  for (auto val = 1; val <= 9; ++val) {
    auto col = val - 1;
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(val, Kind::Sou);
      deck.emplace_back(val, Kind::Pin);
      deck.emplace_back(val, Kind::Man);
    }
  }

  WindKind winds[4] =
    {WindKind::East, WindKind::South, WindKind::West, WindKind::North};
  for (auto col = 0; col < 4; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(winds[col]), Kind::Wind);
    }
  }

  DragonKind dragons[3] =
    {DragonKind::Red, DragonKind::Green, DragonKind::White};
  for (auto col = 0; col < 3; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(dragons[col]), Kind::Dragon);
    }
  }

  SeasonKind seasons[4] =
    {SeasonKind::Spring, SeasonKind::Summer, SeasonKind::Autumn, SeasonKind::Winter};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(seasons[col]), Kind::Season);
  }

  FlowerKind flowers[4] =
    {FlowerKind::Orchid, FlowerKind::Plum, FlowerKind::Mum, FlowerKind::Bamboo};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(flowers[col]), Kind::Flower);
  }
}

void Game::handle_click(Point<float> click) {
  const auto [mouse_x, mouse_y] = click;

  optional<Position> clicked_tile = nullopt;
  for (size_t z = board.layers(); z-- > 0; ) {
    const auto [grid_x, grid_y] = graphics.resolve_click(mouse_x, mouse_y, z);

    if (grid_y >= board.length() || grid_x >= board.width()) {
      continue;
    }

    if (board.tile({grid_x, grid_y, z})) {
      clicked_tile = board.get_origin({grid_x, grid_y, z});
      break;
    }
  }

  if (!clicked_tile || !board.check_open(*clicked_tile)) {
    board.selected = nullopt;
    return;
  }

  if (board.selected && board.check_equal(*board.selected, *clicked_tile)) {
    auto a_pos = *clicked_tile;
    auto b_pos = *board.selected;
    history.record(a_pos, board.tile(a_pos), b_pos, board.tile(b_pos));

    board.remove_tile(*clicked_tile);
    board.remove_tile(*board.selected);
    board.selected = nullopt;
    return;
  }

  board.selected = *clicked_tile;
}

void Game::handle_undo() {
  auto point = history.rewind();
  if (!point)
    return;

  const auto [l, r] = *point;
  board.restore_tile(l.second, l.first);
  board.restore_tile(r.second, r.first);

  board.selected = nullopt;
}

void Game::handle_redo() {
  const auto point = history.restore();
  if (!point)
    return;

  const auto [l, r] = *point;
  board.remove_tile(l.first);
  board.remove_tile(r.first);

  board.selected = nullopt;
}
