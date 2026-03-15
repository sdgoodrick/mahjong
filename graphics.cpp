#include "graphics.h"

#include <iostream>

void Graphics::draw_tile(const Tile& t, Position p) {
  if (!tiles.contains(t)) {
    std::cerr << "unexpected tile: {" << static_cast<unsigned short>(t.kind) << ", " << t.value << "}" << std::endl;
    return;
  }

  auto draw_x = p.x * (cfg.tile_logical_w / 2) - (p.z * cfg.z_offset);
  auto draw_y = p.y * (cfg.tile_logical_h / 2) - (p.z * cfg.z_offset);

  auto sprite = tiles.find(t)->second;
  render(tileset, draw_x, draw_y, &sprite);
}

void Graphics::render(Texture& tex, float x, float y, SDL_FRect* clip) {
  SDL_FRect dest = {x, y, static_cast<float>(tex.width()), static_cast<float>(tex.height())};
  if (clip != nullptr) {
    dest.w = static_cast<float>(clip->w);
    dest.h = static_cast<float>(clip->h);
  }

  SDL_RenderTexture(ren, tex.texture(), clip, &dest);
}

void Graphics::set_bg() {
  const auto [r, g, b] = cfg.bg_color;
  SDL_SetRenderDrawColor(ren, r, g, b, 255);
}

void Graphics::clear() {
  SDL_RenderClear(ren);
}

void Graphics::update() {
  SDL_RenderPresent(ren);
}

void Graphics::draw_selection(Position pos) {
    auto w = cfg.tile_logical_w;
    auto h = cfg.tile_logical_h;
    auto draw_x = pos.x * (w / 2) - (pos.z * cfg.z_offset);
    auto draw_y = pos.y * (h / 2) - (pos.z * cfg.z_offset);

    const auto [r, g, b] = cfg.selection_color;
    SDL_SetRenderDrawColor(ren, r, g, b, 255);
    SDL_FRect selection = {draw_x, draw_y, w, h};
    draw_rect(&selection, cfg.selection_thickness);
}

void Graphics::draw_rect(const SDL_FRect* rect, float thickness) {
  SDL_FRect top = { rect->x, rect->y, rect->w, thickness };
  SDL_RenderFillRect(ren, &top);

  SDL_FRect bottom = { rect->x, rect->y + rect->h - thickness, rect->w, thickness };
  SDL_RenderFillRect(ren, &bottom);

  // Left side shrunk slightly to avoid overlapping the top/bottom corners twice
  SDL_FRect left = { rect->x, rect->y + thickness, thickness, rect->h - (thickness * 2) };
  SDL_RenderFillRect(ren, &left);

  SDL_FRect right = {
    rect->x + rect->w - thickness,
    rect->y + thickness,
    thickness,
    rect->h - (thickness * 2)
  };

  SDL_RenderFillRect(ren, &right);
}

bool Graphics::init() {
  SDL_Init(SDL_INIT_VIDEO);

  win = SDL_CreateWindow("Mahjigg", 800, 600, 0);
  if (win == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  ren = SDL_CreateRenderer(win, NULL);
  if (ren == nullptr) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(win);
    SDL_Quit();
    return false;
  }

  // We'll log this and move on. If you can run the game without VSync
  // on your windows98 workstation, go for it.
  if (!SDL_SetRenderVSync(ren, SDL_RENDERER_VSYNC_ADAPTIVE)) {
    std::cerr << "Failed to set VSync: " << SDL_GetError() << std::endl;
  }

  if (!load_media())
    return false;

  init_tileset();
  return true;
}

void Graphics::init_tileset() {
  auto get_sprite = [=, this](int col, int row) -> SDL_FRect {
    return {
      10.0f + (col * cfg.sheet_spacing),
      2.0f + (row * cfg.sheet_spacing),
      cfg.tile_visual_w,
      cfg.tile_visual_h
    };
  };

  for (unsigned short val = 1; val <= 9; ++val) {
    auto col = val - 1;
    tiles[{val, Kind::Sou}] = get_sprite(col, 0);
    tiles[{val, Kind::Pin}] = get_sprite(col, 1);
    tiles[{val, Kind::Man}] = get_sprite(col, 2);
  }

  WindKind winds[4] = {WindKind::East, WindKind::South, WindKind::West, WindKind::North};
  for (auto col = 0; col < 4; ++col)
    tiles[{static_cast<unsigned short>(winds[col]), Kind::Wind}] = get_sprite(col, 3);

  DragonKind dragons[3] = {DragonKind::Red, DragonKind::Green, DragonKind::White};
  for (unsigned short col = 0; col < 3; ++col)
    tiles[{static_cast<unsigned short>(dragons[col]), Kind::Dragon}] = get_sprite(col + 4, 3);

  SeasonKind seasons[4] = {SeasonKind::Spring, SeasonKind::Summer, SeasonKind::Autumn, SeasonKind::Winter};
  for (unsigned short col = 0; col < 4; ++col)
    tiles[{static_cast<unsigned short>(seasons[col]), Kind::Season}] = get_sprite(col, 4);

  FlowerKind flowers[4] = {FlowerKind::Orchid, FlowerKind::Plum, FlowerKind::Mum, FlowerKind::Bamboo};
  for (unsigned short col = 0; col < 4; ++col)
    tiles[{static_cast<unsigned short>(flowers[col]), Kind::Flower}] = get_sprite(col + 4, 4);
}

bool Graphics::load_media() {
  if (!tileset.load_from_file(ren, "../assets/deck.png")) {
    return false;
  };

  return true;
}

float Graphics::tile_width() const {
  return cfg.tile_logical_w;
}

float Graphics::tile_height() const {
  return cfg.tile_logical_h;
}

std::pair<std::size_t, std::size_t> Graphics::resolve_click(float mouse_x, float mouse_y, std::size_t z) const {
  float z_adjusted_x = mouse_x + (z * cfg.z_offset);
  float z_adjusted_y = mouse_y + (z * cfg.z_offset);

  auto point = std::make_pair(
        static_cast<std::size_t>(z_adjusted_x / (cfg.tile_logical_w / 2)),
	static_cast<std::size_t>(z_adjusted_y / (cfg.tile_logical_h / 2))
  );

  return point;
}

void Graphics::shutdown() {
  tileset.destroy();
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
}

Graphics::~Graphics() {
  shutdown();
}
