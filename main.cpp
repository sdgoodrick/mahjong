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
#include <map>

#include "board.h"
#include "game.h"

#include "graphics.h"

int main(int argc, char* argv[]) {
  Graphics g;
  if (!g.init())
    return 1;

  Game game;

  SDL_Event e;
  bool quit = false;

  while (!quit) {
    g.set_bg();
    g.clear();

    game.draw_tiles(g);
    game.draw_selection(g);

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
	quit = true;
      } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	game.handle_click(g, {mouse_x, mouse_y});
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
	if (e.key.mod & SDL_KMOD_CTRL && e.key.scancode == SDL_SCANCODE_Z)
	  game.handle_undo();
	else if (e.key.mod & SDL_KMOD_CTRL && e.key.scancode == SDL_SCANCODE_Y)
	  game.handle_redo();
      }
    }
	
    g.update();
  }


  g.shutdown();
  SDL_Quit();

  return 0;
}
