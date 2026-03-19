#pragma once

#include <SDL3/SDL.h>

#include <string>

struct Texture {
  ~Texture();

  bool load_from_file(SDL_Renderer* ren, std::string path);
  bool load_from_surface(SDL_Renderer *ren, SDL_Surface* surf);
  void destroy();

  int width() const;
  int height() const;
  SDL_Texture* texture();
private:
  SDL_Texture* tex = nullptr;

  int w = 0;
  int h = 0;
};
