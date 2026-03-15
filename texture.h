#pragma once

#include <SDL3/SDL.h>

#include <string>

struct Texture {
  ~Texture();

  bool load_from_file(SDL_Renderer* ren, std::string path);
  void destroy();

  int width() const;
  int height() const;
  SDL_Texture* texture();
private:
  SDL_Texture* tex;

  int w;
  int h;
};
