#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cairo/cairo.h>
#include <math.h>

#include "constants.h"
#include "gamestate.h"

double fakeDist(double x1, double y1, double x2, double y2) {
  const double dx = x1 - x2;
  const double dy = y1 - y2;
  return dx * dx + dy * dy;
}

void selectNearest(Gamestate* gs, int x, int y) {
  // default: nothing selected
  gs->selected.type = TYPE_NONE;

  // nodes
  for (int i = 0; i < gs->numNodes; i++) {
    Node* current = gs->nodes + i;
    if (fakeDist(x, y, current->x, current->y)
        < SELECTION_DISTANCE * SELECTION_DISTANCE
    ) {
      gs->selected.type = TYPE_NODE;
      gs->selected.value.node = current;
    }
  }

  // edges
  //TODO implement
}

void applyFunction(Gamestate* gs, int x, int y) {
  if (gs->selected.type == TYPE_NODE) {
    if (gs->numNodes < MAX_NODES) {
      gs->nodes[gs->numNodes] = (Node) {
        .color = COLOR_WHITE,
        .x = x,
        .y = y,
      };
      gs->numNodes++;
      gs->selected.value.node->color = !gs->selected.value.node->color;
      //TODO add edge
    } else {
      //TODO error handling
      perror("max number of nodes reached");
    }
  } else if (gs->selected.type == TYPE_EDGE) {
    //TODO implement
  }
}

void handleEvents(Gamestate* gs) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_QUIT:
        gs->quit = true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_LSHIFT:
            gs->shiftPressed = true;
            break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
          case SDLK_LSHIFT:
            gs->shiftPressed = false;
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          if (!gs->shiftPressed) {
            selectNearest(gs, event.button.x, event.button.y);
          } else {
            applyFunction(gs, event.button.x, event.button.y);
          }
        }
        break;
    }
  }
}

void render(SDL_Renderer* r, Gamestate* gs) {
  const SDL_Rect windowDimensions = {
    .w = WINDOW_WIDTH,
    .h = WINDOW_HEIGHT,
    .x = 0,
    .y = 0,
  };

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
    return;
  }

  cairo_surface_t* cairoSurf = cairo_image_surface_create_for_data(
      sdlSurf->pixels,
      CAIRO_FORMAT_RGB24,
      windowDimensions.w,
      windowDimensions.h,
      sdlSurf->pitch
  );

  cairo_t* cr = cairo_create(cairoSurf);

  cairo_save(cr);

  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_paint(cr);

  // render nodes
  for (int i = 0; i < gs->numNodes; i++) {
    cairo_save(cr);
    Node* current = gs->nodes + i;
    if (current->color == COLOR_BLACK) {
      cairo_set_source_rgb(cr, 0, 0, 0);
    } else {
      cairo_set_source_rgb(cr, 1, 1, 1);
    }

    cairo_translate(cr, current->x, current->y);
    cairo_arc(cr, 0, 0, NODE_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_restore(cr);
  }

  // render selection
  cairo_restore(cr);
  if (gs->selected.type == TYPE_NODE) {
    cairo_set_source_rgba(cr, 1, 0, 0, 0.5);
    cairo_translate(cr, gs->selected.value.node->x, gs->selected.value.node->y);
    cairo_arc(cr, 0, 0, NODE_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);
  } else if (gs->selected.type == TYPE_EDGE) {
    //TODO implement
  }

  cairo_surface_destroy(cairoSurf);

  SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(r, sdlSurf);
  SDL_FreeSurface(sdlSurf);
  SDL_RenderCopy(r, sdlTexture, &windowDimensions, &windowDimensions);
  SDL_DestroyTexture(sdlTexture);

  SDL_RenderPresent(r);
}

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

  Gamestate gs;
  gamestate_init(&gs);

  while (!gs.quit) {
    handleEvents(&gs);
    render(r, &gs);
  }

  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
