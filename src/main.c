#include "constants.h"

#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cairo/cairo.h>

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
  cairo_t* cr;

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          quit = true;
          break;
      }
    }

    SDL_Rect windowDimensions = {0};
    SDL_GetWindowSize(window, &windowDimensions.w, &windowDimensions.h);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderClear(r);

    SDL_Surface* sdlSurf = SDL_CreateRGBSurface(
        0,
        windowDimensions.w,
        windowDimensions.h,
        32,
        0x00ff0000,
        0x0000ff00,
        0x000000ff,
        0x0
    );
    if (sdlSurf == NULL) {
      perror("Could not create RGB Surface");
      return EXIT_FAILURE;
    }

    cairo_surface_t* cairoSurf = cairo_image_surface_create_for_data(
        sdlSurf->pixels,
        CAIRO_FORMAT_RGB24,
        windowDimensions.w,
        windowDimensions.h,
        sdlSurf->pitch
    );
    cr = cairo_create(cairoSurf);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    cairo_surface_destroy(cairoSurf);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(r, sdlSurf);
    SDL_FreeSurface(sdlSurf);
    const SDL_Rect rect = {
      .w = windowDimensions.w,
      .h = windowDimensions.h,
      .x = 0,
      .y = 0
    };
    SDL_RenderCopy(r, sdlTexture, &rect, &rect);
    SDL_DestroyTexture(sdlTexture);

    SDL_RenderPresent(r);
  }

  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
