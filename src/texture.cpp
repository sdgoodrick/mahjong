#include "texture.h"

#include <SDL3_image/SDL_image.h>

void Texture::destroy() {
  SDL_DestroyTexture(tex);
  w = h = 0;
}

int Texture::width() const {
  return w;
}

int Texture::height() const {
  return h;
}

SDL_Texture* Texture::texture() {
  return tex;
}

bool Texture::load_from_file(SDL_Renderer* ren, std::string path) {
  destroy();

  SDL_Surface* loaded = IMG_Load(path.c_str());
  if (loaded == nullptr) {
    SDL_Log("failed to load image %s: %s\n", path.c_str(), SDL_GetError());
    return false;
  }

  tex = SDL_CreateTextureFromSurface(ren, loaded);
  if (tex == nullptr) {
    SDL_Log("failed to create texture from loaded pixels: %s\n", SDL_GetError());
    return false;
  }

  w = loaded->w;
  h = loaded->h;
  SDL_DestroySurface(loaded);
  return true;
}

bool Texture::load_from_surface(SDL_Renderer *ren, SDL_Surface* surf) {
  if (tex = SDL_CreateTextureFromSurface(ren, surf); tex == nullptr) {
    SDL_Log("failed to create texture from surface: %s\n", SDL_GetError());
    return false;
  }

  SDL_DestroySurface(surf);
  surf = nullptr;

  w = tex->w;
  h = tex->h;
  return true;
}

Texture::~Texture() {
  destroy();
}
