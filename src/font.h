#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "texture.h"
#include "util.h"

struct Font {
  Font() = default;

  bool open(const std::string& path, int pt);
  void close();

  Texture render(SDL_Renderer* ren, const std::string& t, const Color& c);

private:
  TTF_Font* font = nullptr;
};
