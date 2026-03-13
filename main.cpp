#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <ranges>
#include <vector>
#include <set>
#include <tuple>

#include "texture.h"
#include "board.h"

using namespace std;

template<typename T>
using Point = tuple<T, T>;

SDL_Renderer* ren;
Texture spritesheet;
const float z_offset_x = 3.0f; // Shift left
const float z_offset_y = 3.0f; // Shift up

bool check_open(Layer<Tile*> layer, size_t x, size_t y);

bool load_media(SDL_Renderer* ren);
void render(Texture& tex, float x, float y, SDL_FRect* clip);
void draw_rect(SDL_Renderer* renderer, const SDL_FRect* rect, float thickness);
void draw_tiles(SDL_Renderer* ren, const Board& board);
void render_selection(SDL_Renderer* ren, const Board& board, bool& selection_drawn);
void handle_click(Board& board, Point<float> mouse);
std::vector<Tile> create_deck();

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* win = SDL_CreateWindow("SDL3 Project", 800, 600, 0);
  if (win == nullptr) {
    cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }

  ren = SDL_CreateRenderer(win, NULL);
  if (ren == nullptr) {
    cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  if (!SDL_SetRenderVSync(ren, SDL_RENDERER_VSYNC_ADAPTIVE)) {
    cerr << "Failed to set VSync: " << SDL_GetError() << endl;
  }

  load_media(ren);

  SDL_Event e;
  bool quit = false;

  auto deck = create_deck();

  random_device rd;
  mt19937 g(rd());
  ranges::shuffle(deck, g);
  Board board("../layouts/turtle", deck);

  while (!quit) {
    SDL_SetRenderDrawColor(ren, 34, 139, 34, 255); // Set render draw color to muted green
    SDL_RenderClear(ren); // Clear the renderer

    draw_tiles(ren, board);

    bool selection_drawn = false;
    if (board.selected_tile)
      render_selection(ren, board, selection_drawn);

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
	quit = true;
      } else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	handle_click(board, {mouse_x, mouse_y});
      }
    }
	
    SDL_RenderPresent(ren); // Render the screen
  }

  spritesheet.destroy();

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}

bool load_media(SDL_Renderer* ren) {
  if (!spritesheet.load_from_file(ren, "../assets/deck.png")) {
    return false;
  };

  return true;
}

void render(Texture& tex, float x, float y, SDL_FRect* clip) {
  SDL_FRect dest = {x, y, static_cast<float>(tex.width()), static_cast<float>(tex.height())};
  if (clip != nullptr) {
    dest.w = static_cast<float>(clip->w);
    dest.h = static_cast<float>(clip->h);
  }

  SDL_RenderTexture(ren, tex.texture(), clip, &dest);
}

std::vector<Tile> create_deck() {
  std::vector<Tile> deck;
  deck.reserve(144);

  const float visual_w = 47.0f;
  const float visual_h = 63.0f;

  auto get_sprite = [=](int col, int row) -> SDL_FRect {
    return { 10.0f + (col * 64.0f), 2.0f + (row * 64.0f), visual_w, visual_h };
  };

  for (auto val = 1; val <= 9; ++val) {
    auto col = val - 1;
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(val, Kind::Sou, get_sprite(col, 0));
      deck.emplace_back(val, Kind::Pin, get_sprite(col, 1));
      deck.emplace_back(val, Kind::Man, get_sprite(col, 2));
    }
  }

  WindKind winds[4] = {WindKind::East, WindKind::South, WindKind::West, WindKind::North};
  for (auto col = 0; col < 4; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(winds[col]), Kind::Wind, get_sprite(col, 3));
    }
  }

  DragonKind dragons[3] = {DragonKind::Red, DragonKind::Green, DragonKind::White};
  for (auto col = 0; col < 3; ++col) {
    for (auto i = 0; i < 4; ++i) {
      deck.emplace_back(static_cast<unsigned short>(dragons[col]), Kind::Dragon, get_sprite(col + 4, 3));
    }
  }

  SeasonKind seasons[4] = {SeasonKind::Spring, SeasonKind::Summer, SeasonKind::Autumn, SeasonKind::Winter};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(seasons[col]), Kind::Season, get_sprite(col, 4));
  }

  FlowerKind flowers[4] = {FlowerKind::Orchid, FlowerKind::Plum, FlowerKind::Mum, FlowerKind::Bamboo};
  for (auto col = 0; col < 4; ++col) {
    deck.emplace_back(static_cast<unsigned short>(flowers[col]), Kind::Flower, get_sprite(col + 4, 4));
  }

  return deck;
}

void draw_rect(SDL_Renderer* renderer, const SDL_FRect* rect, float thickness) {
  // Top side
  SDL_FRect top = { rect->x, rect->y, rect->w, thickness };
  SDL_RenderFillRect(renderer, &top);

  // Bottom side
  SDL_FRect bottom = { rect->x, rect->y + rect->h - thickness, rect->w, thickness };
  SDL_RenderFillRect(renderer, &bottom);

  // Left side (shrunk slightly to avoid overlapping the top/bottom corners twice)
  SDL_FRect left = { rect->x, rect->y + thickness, thickness, rect->h - (thickness * 2) };
  SDL_RenderFillRect(renderer, &left);

  // Right side
  SDL_FRect right = { rect->x + rect->w - thickness, rect->y + thickness, thickness, rect->h - (thickness * 2) };
  SDL_RenderFillRect(renderer, &right);
}

Point<size_t> get_origin(Layer<Tile*> layer, size_t x, size_t y) {
  auto origin_x = x;
  auto origin_y = y;

  if (origin_x > 0 && layer[origin_y][origin_x - 1] == layer[y][x]) {
    origin_x -= 1;
  }

  if (origin_y > 0 && layer[origin_y - 1][origin_x] == layer[y][x]) {
    origin_y -= 1;
  }

  return {origin_x, origin_y};
}

bool check_open(Layer<Tile*> layer, size_t x, size_t y) {
  const auto [origin_x, origin_y] = get_origin(layer, x, y);
  const auto max_x = layer[0].size();
  const auto max_y = layer.size();

  // Left-most edge
  if (origin_x == 0)
    return true;

  bool top_left_open = layer[origin_y][origin_x - 1] == nullptr;
  bool bottom_left_open = (origin_y + 1 >= max_y) || layer[origin_y+1][origin_x - 1] == nullptr;
  if (top_left_open && bottom_left_open)
    return true;

  // Right-most edge
  if (origin_x + 2 >= max_x)
    return true;

  bool top_right_open = layer[origin_y][origin_x + 2] == nullptr;
  bool bottom_right_open = (origin_y + 1 >= max_y) || layer[origin_y + 1][origin_x + 2] == nullptr;
  if (top_right_open && bottom_right_open)
    return true;
 
  return false;
}

void render_selection(SDL_Renderer* ren, const Board& board, bool& selection_drawn) {
  for (int z = 0; z < board.tiles.size(); ++z) {
    for (int y = 0; y < board.tiles[z].size(); ++y) {
      for (int x = 0; x < board.tiles[z][y].size(); ++x) {
	if (board.tiles[z][y][x] == board.selected_tile) {
	  const auto [origin_x, origin_y] = get_origin(board.tiles[z], x, y);
	 
	  if (!selection_drawn) {
	    auto w = board.tile_width();
	    auto h = board.tile_height();

	    auto draw_x = origin_x * (w / 2);
	    auto draw_y = origin_y * (h / 2);

	    draw_x -= (z * z_offset_x);
	    draw_y -= (z * z_offset_y);

	    SDL_SetRenderDrawColor(ren, 70, 130, 180, 255); 
	    SDL_FRect selection = {draw_x, draw_y, w, h};
	    draw_rect(ren, &selection, 3.0f);
	    selection_drawn = true;
	  }
	}
      }
    }
  }
}

void draw_tiles(SDL_Renderer* ren, const Board& board) {
  std::set<Tile*> seen;
  for (int z = 0; z < board.tiles.size(); ++z) {
    for (int y = 0; y < board.tiles[z].size(); ++y) {
      for (int x = 0; x < board.tiles[z][y].size(); ++x) {
	Tile* t = board.tiles[z][y][x];
	if (t != nullptr && !seen.contains(t)) {
	  seen.insert(t);
	  auto real_x = x * (board.tile_width() / 2) - (z * z_offset_x);
	  auto real_y = y * (board.tile_height() / 2) - (z * z_offset_y);
	  render(spritesheet, real_x, real_y, &t->sprite);
	}
      }
    }
  }
}

void remove_tile(Board& board, Tile* t) {
  for (int z = 0; z < board.tiles.size(); ++z) {
    for (int y = 0; y < board.tiles[z].size(); ++y) {
      for (int x = 0; x < board.tiles[z][y].size(); ++x) {
	if (board.tiles[z][y][x] == t)
	  board.tiles[z][y][x] = nullptr;
      }
    }
  }
}

void handle_click(Board& board, Point<float> mouse) {
  const auto [mouse_x, mouse_y] = mouse;
  auto x = static_cast<size_t>(mouse_x) / (board.tile_width() / 2);
  auto y = static_cast<size_t>(mouse_y) / (board.tile_height() / 2);

  Tile* found_tile = nullptr;
  Layer<Tile*> found_layer;
  
  for (int z = board.tiles.size() - 1; z >= 0; --z) {
    // Adjust mouse coordinates to accomodate Z offset
    x = static_cast<size_t>(mouse_x + (z * z_offset_x)) / (board.tile_width() / 2);
    y = static_cast<size_t>(mouse_y + (z * z_offset_y)) / (board.tile_height() / 2);;
    auto layer = board.tiles[z];
    if (y < layer.size() && x < layer[0].size()) {
      if (layer[y][x]) {
	found_tile = layer[y][x];
	found_layer = layer;
	break;
      }
    }
  }

  if (found_tile) {
    if (check_open(found_layer, x, y)) {
      if (auto* t = board.selected_tile; t && t != found_tile && *t == *found_tile) {
	remove_tile(board, found_tile);
	remove_tile(board, board.selected_tile);
	board.selected_tile = nullptr;
      } else {
	board.selected_tile = found_tile;
      }
    } else {
      board.selected_tile = nullptr;
    }
  } else {
    board.selected_tile = nullptr;
  }
}
