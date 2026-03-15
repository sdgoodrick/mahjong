#pragma once

#include <SDL3/SDL.h>

#include <map>
#include <utility>

#include "texture.h"
#include "tile.h"
#include "util.h"

using Color = std::size_t[3];

// Holds various configuration settings for the graphics manager.
// Currently hard-coded for convenience, but could be made configurable
struct GraphicsConfig {
  // The size of a tile including its edges
  const float tile_visual_w = 47.0f;
  const float tile_visual_h = 63.0f;

  // The size of a tile face
  const float tile_logical_w = 44.0f;
  const float tile_logical_h = 60.0f;

  // The shift up & left used per z layer to give illusion of depth
  const float z_offset = 3.0f;

  // Space between the tiles and the window border, in pixels
  const float x_padding = 50.0f;
  const float  y_padding = 50.0f;

  // The size of spaces in the tileset spritesheet
  const float sheet_spacing = 64.0f;

  // The size of the lines on the cursor
  const float selection_thickness = 3.0f;

  const Color bg_color = {34, 139, 34};
  const Color selection_color = {70, 130, 180};

  const float screen_width = 800;
  const float screen_height = 600;
};

struct Graphics {
  bool init();
  void shutdown();
  void draw_tile(const Tile& t, Position p);
  void draw_selection(Position pos);
  void render(Texture& tex, float x, float y, SDL_FRect* clip);
  void set_bg();
  void clear();
  void update();

  Point<std::size_t> padding() const {
    return {cfg.x_padding, cfg.y_padding};
  };

  ~Graphics();

  std::pair<std::size_t, std::size_t> resolve_click(float mouse_x, float mouse_y, std::size_t z) const;

  float tile_width() const;
  float tile_height() const;

private:
  bool load_media();
  void init_tileset();
  void draw_rect(const SDL_FRect* rect, float thickness);

private:
  std::map<Tile, SDL_FRect> tiles;

  SDL_Renderer* ren;
  SDL_Window* win;
  Texture tileset;
  GraphicsConfig cfg;
};
