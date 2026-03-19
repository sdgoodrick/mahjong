#include "font.h"

// const std::string p = "../asserts/NotoSerif-VariableFont_wdth,wght.ttf";

bool Font::open(const std::string& path, int pt) {
  font = TTF_OpenFont(path.c_str(), pt);
  return font != nullptr;
}

void Font::close() {
  TTF_CloseFont(font);
  font = nullptr;
}

Texture Font::render(SDL_Renderer* ren, const std::string& t, const Color& c) {
  Texture res;
  if (!font)
    return res;

  const auto [r, g, b] = c;
  SDL_Surface* surf = TTF_RenderText_Blended(font, t.c_str(), 0, {r, g, b, 255});
  if (surf) {
    res.load_from_surface(ren, surf);
  }

  return res;
}
