#include <iostream>

#include "game.h"
#include "graphics.h"

int main(int argc, char* argv[]) {
  Graphics graphics;
  if (!graphics.init())
    return 1;

  Game game(graphics);
  game.update_matches();

  SDL_Event e;
  bool quit = false;

  while (!quit) {
    graphics.set_bg();
    graphics.clear();

    game.draw_tiles();
    std::cout << "\rmatches left: " << game.count_matches() << "\033[K" << std::flush;
    game.draw_selection();

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
	quit = true;
      } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	game.handle_click({mouse_x, mouse_y});
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
	if (e.key.mod & SDL_KMOD_CTRL && e.key.scancode == SDL_SCANCODE_Z)
	  game.handle_undo();
	else if (e.key.mod & SDL_KMOD_CTRL && e.key.scancode == SDL_SCANCODE_Y)
	  game.handle_redo();
	else if (e.key.scancode == SDL_SCANCODE_H);
	  // game.handle_hint();
      }
    }
	
    graphics.update();
  }


  graphics.shutdown();
  return 0;
}
