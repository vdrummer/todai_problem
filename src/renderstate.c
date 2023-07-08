#include "renderstate.h"
#include "constants.h"

void renderstate_init(Renderstate* rs, SDL_Renderer* r) {
  if (rs == NULL) {
    return;
  }

  rs->r = r;
  rs->windowDimensions = (SDL_Rect) {
    .w = WINDOW_WIDTH,
    .h = WINDOW_HEIGHT,
    .x = 0,
    .y = 0,
  };

  SDL_RenderClear(r);

  rs->sdlSurf = SDL_CreateRGBSurface(
      0,
      rs->windowDimensions.w,
      rs->windowDimensions.h,
      32,
      0x00ff0000,
      0x0000ff00,
      0x000000ff,
      0x0
  );
  if (rs->sdlSurf == NULL) {
    perror("Could not create RGB Surface");
    return;
  }

  rs->cairoSurf = cairo_image_surface_create_for_data(
      rs->sdlSurf->pixels,
      CAIRO_FORMAT_RGB24,
      rs->windowDimensions.w,
      rs->windowDimensions.h,
      rs->sdlSurf->pitch
  );

  rs->cr = cairo_create(rs->cairoSurf);
}

void renderstate_render(Renderstate* rs) {
  SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(rs->r, rs->sdlSurf);
  SDL_RenderCopy(rs->r, sdlTexture, &rs->windowDimensions, &rs->windowDimensions);
  SDL_DestroyTexture(sdlTexture);

  SDL_RenderPresent(rs->r);
}
