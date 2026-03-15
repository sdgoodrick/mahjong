#include "game.h"

#include <algorithm>
#include <random>
#include <ranges>
#include <set>

Game::Game() {
  create_deck();

  std::random_device rd;
  std::mt19937 g(rd());
  std::ranges::shuffle(deck, g);
  board.init("../layouts/turtle", deck);
}

void Game::draw_tiles(Graphics& g) {
  std::set<Tile*> seen;
  for (std::size_t z = 0; z < board.layers(); ++z) {
    for (std::size_t y = 0; y < board.length(); ++y) {
      for (std::size_t x = 0; x < board.width(); ++x) {
	Tile* t = board.get_tile({x, y, z});
	if (t != nullptr && !seen.contains(t)) {
	  seen.insert(t);
	  g.draw_tile(*t, {x, y, z});
	}
      }
    }
  }
}

void Game::draw_selection(Graphics& g) {
  if (board.selected)
    g.draw_selection(board.selected.value());
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

  WindKind winds[4] = {WindKind::East, WindKind::South, WindKind::West, WindKind::North};
  for (auto col = 0; col < 4; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(winds[col]), Kind::Wind);
    }
  }

  DragonKind dragons[3] = {DragonKind::Red, DragonKind::Green, DragonKind::White};
  for (auto col = 0; col < 3; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(dragons[col]), Kind::Dragon);
    }
  }

  SeasonKind seasons[4] = {SeasonKind::Spring, SeasonKind::Summer, SeasonKind::Autumn, SeasonKind::Winter};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(seasons[col]), Kind::Season);
  }

  FlowerKind flowers[4] = {FlowerKind::Orchid, FlowerKind::Plum, FlowerKind::Mum, FlowerKind::Bamboo};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(flowers[col]), Kind::Flower);
  }
}

void Game::handle_click(Graphics& g, Point<float> click) {
  const auto [mouse_x, mouse_y] = click;

  std::optional<Position> clicked_tile = std::nullopt;
  for (int signed_z = board.layers() - 1; signed_z >= 0; --signed_z) {
    auto z = static_cast<std::size_t>(signed_z);
    const auto [grid_x, grid_y] = g.resolve_click(mouse_x, mouse_y, z);

    if (grid_y >= board.length() || grid_x >= board.width()) {
      break;
    }

    if (board.get_tile({grid_x, grid_y, z})) {
      clicked_tile = board.get_origin({grid_x, grid_y, z});
      break;
    }
  }

  if (!clicked_tile || !board.check_open(*clicked_tile)) {
    board.selected = std::nullopt;
    return;
  }

  if (board.selected && board.check_equal(*board.selected, *clicked_tile)) {
    auto a_pos = *clicked_tile;
    auto b_pos = *board.selected;
    history.record(a_pos, board.get_tile(a_pos), b_pos, board.get_tile(b_pos));

    board.remove_tile(clicked_tile.value());
    board.remove_tile(board.selected.value());
    board.selected = std::nullopt;
    return;
  }

  board.selected = *clicked_tile;
}

void Game::handle_undo() {
  auto point = history.rewind();
  if (!point)
    return;

  const auto [l, r] = point.value();
  board.restore_tile(l.second, l.first);
  board.restore_tile(r.second, r.first);

  board.selected = std::nullopt;
}

void Game::handle_redo() {
  const auto point = history.restore();
  if (!point)
    return;

  const auto [l, r] = point.value();
  board.remove_tile(l.first);
  board.remove_tile(r.first);

  board.selected = std::nullopt;
}

std::optional<Match> History::rewind() {
   if (matches.empty() || cursor == 0)
     return std::nullopt;

   auto match = matches[--cursor];
   return match;
}

std::optional<Match> History::restore() {
  if (matches.empty() || cursor >= matches.size())
    return std::nullopt;

   auto match = matches[cursor++];
   return match;
}

Match::Match(Position a_pos, Tile* a_tile, Position b_pos, Tile* b_tile)
  : a(std::make_pair(a_pos, a_tile)),
    b(std::make_pair(b_pos, b_tile))
{}
