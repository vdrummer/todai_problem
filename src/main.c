#include "constants.h"

#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

int main(void) {
  if (SDL_Init(0) < 0) {
    perror("Could not init SDL");
    return EXIT_FAILURE;
  }
  SDL_Window* window = SDL_CreateWindow(
      "Todai Graph Problem",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      0
  );
  if (window == NULL) {
    perror("Could not create SDL window.");
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Renderer* r = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
  );
  if (r == NULL) {
    perror("Could not create SDL renderer.");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  bool quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          quit = true;
          break;
      }
    }

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderClear(r);

    SDL_RenderPresent(r);
  }

  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
