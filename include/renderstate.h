#ifndef TODAIGRAPH_RENDERSTATE_H
#define TODAIGRAPH_RENDERSTATE_H

#include <SDL2/SDL.h>
#include <cairo/cairo.h>

typedef struct renderstate Renderstate;
struct renderstate {
  SDL_Renderer* r;
  cairo_t* cr;

  SDL_Surface* sdlSurf;
  cairo_surface_t* cairoSurf;
  SDL_Rect windowDimensions;
};

void renderstate_init(Renderstate* rs, SDL_Renderer* r);
void renderstate_render(Renderstate* rs);
void renderstate_destroy(Renderstate* rs);

#endif // TODAIGRAPH_RENDERSTATE_H
