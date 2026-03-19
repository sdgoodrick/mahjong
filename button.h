#pragma once

#include <SDL3/SDL.h>

#include "texture.h"
#include "util.h"

namespace ui {

struct Boundary {
  SDL_FRect bounds;
  bool contains(float x, float y) const;
};

enum class ButtonKind {
  Start,
  Settings,
};

struct Button {
  ButtonKind kind;
  Boundary boundary;
  bool hovered = false;
  bool pressed = false;
};


struct ButtonClip {
  enum Kind {
    Default,
    Hovered,
    Pressed,
    All
  };
};

struct ButtonView {
  ButtonKind kind;
  SDL_FRect clip[ButtonClip::All];
};

} // namespace ui
