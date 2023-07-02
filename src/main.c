#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cairo/cairo.h>
#include <math.h>

#include "constants.h"
#include "gamestate.h"
#include "mathstuff.h"

#define GET_NODE_X(n) (n / MAX_NODES)
#define GET_NODE_Y(n) (n % MAX_NODES)
#define GET_NODE_XY(x, y) (MAX_NODES * y + x)

void selectNearest(Gamestate* gs, int x, int y) {
  // default: nothing selected
  gs->selected.type = TYPE_NONE;

  // nodes
  for (int i = 0; i < gs->numNodes; i++) {
    Node* current = gs->nodes + i;
    if (ms_squaredDist((Point) {x, y}, (Point) {current->x, current->y})
        < SELECTION_RADIUS * SELECTION_RADIUS
    ) {
      gs->selected.type = TYPE_NODE;
      gs->selected.value.node = current;
    }
  }

  // edges
  for (int i = 0; i < MAX_NODES * MAX_NODES; i++) {
    if (gs->edges[i] > 0) {
      Node* n1 = gs->nodes + GET_NODE_X(i);
      Node* n2 = gs->nodes + GET_NODE_Y(i);
      const Point normal = ms_unitNormalVector(
          (Point) {n1->x, n1->y},
          (Point) {n2->x, n2->y}
      );

      const Point a = {
        n1->x + normal.x * SELECTION_DIST,
        n1->y + normal.y * SELECTION_DIST,
      };

      const Point b = {
        n1->x - normal.x * SELECTION_DIST,
        n1->y - normal.y * SELECTION_DIST,
      };

      const Point c = {
        n2->x - normal.x * SELECTION_DIST,
        n2->y - normal.y * SELECTION_DIST,
      };

      const Point d = {
        n2->x + normal.x * SELECTION_DIST,
        n2->y + normal.y * SELECTION_DIST,
      };

      if (ms_pointInRectangle((Point) {x, y}, a, b, c, d)) {
        gs->selected.type = TYPE_EDGE;
        gs->selected.value.edge = i;
      }
    }
  }
}

void applyFunction(Gamestate* gs, int x, int y) {
  if (gs->selected.type == TYPE_NODE) {
    if (gs->numNodes < MAX_NODES) {
      Node* currentNode = gs->selected.value.node;
      gs->nodes[gs->numNodes] = (Node) {
        .color = COLOR_WHITE,
        .x = x,
        .y = y,
      };
      currentNode->color = !currentNode->color;

      // add edge
      int currentIndex = (int) (currentNode - gs->nodes);
      // N.B.: only the bottom left half of the matrix is filled
      gs->edges[GET_NODE_XY(currentIndex, gs->numNodes)] = 1;

      gs->numNodes++;
    } else {
      //TODO error handling
      perror("max number of nodes reached");
    }
  } else if (gs->selected.type == TYPE_EDGE) {
    const int selectedIdx = gs->selected.value.edge;
    const int n1Idx = GET_NODE_X(selectedIdx);
    const int n2Idx = GET_NODE_Y(selectedIdx);

    if (gs->numNodes >= MAX_NODES) {
      perror("max number of nodes reached");
      return;
    }

    gs->nodes[gs->numNodes] = (Node) {
      .color = COLOR_WHITE,
      .x = x,
      .y = y,
    };

    gs->edges[selectedIdx] = 0;
    gs->edges[GET_NODE_XY(n1Idx, gs->numNodes)] = 1;
    gs->edges[GET_NODE_XY(n2Idx, gs->numNodes)] = 1;

    gs->nodes[n1Idx].color = !gs->nodes[n1Idx].color;
    gs->nodes[n2Idx].color = !gs->nodes[n2Idx].color;

    gs->selected.type = TYPE_NONE;

    gs->numNodes++;
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
          case SDLK_RSHIFT:
            gs->shiftPressed = true;
            break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
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

  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_paint(cr);

  // render edges
  cairo_set_source_rgb(cr, 0, 0, 0);
  for (int i = 0; i < MAX_NODES * MAX_NODES; i++) {
    if (gs->edges[i] != 0) {
      Node* n1 = gs->nodes + GET_NODE_X(i);
      Node* n2 = gs->nodes + GET_NODE_Y(i);
      cairo_set_line_width(cr, EDGE_WIDTH);
      cairo_move_to(cr, n1->x, n1->y);
      cairo_line_to(cr, n2->x, n2->y);
      cairo_close_path(cr);
      cairo_stroke(cr);
    }
  }

  // seleted edge
  if (gs->selected.type == TYPE_EDGE) {
    Node* n1 = gs->nodes + GET_NODE_X(gs->selected.value.edge);
    Node* n2 = gs->nodes + GET_NODE_Y(gs->selected.value.edge);
    cairo_set_source_rgba(cr, 1, 0, 0, 0.5);
    cairo_set_line_width(cr, EDGE_WIDTH);
    cairo_move_to(cr, n1->x, n1->y);
    cairo_line_to(cr, n2->x, n2->y);
    cairo_close_path(cr);
    cairo_stroke(cr);
  }

  // render nodes
  for (int i = 0; i < gs->numNodes; i++) {
    Node* current = gs->nodes + i;
    if (current->color == COLOR_BLACK) {
      cairo_set_source_rgb(cr, 0, 0, 0);
    } else {
      cairo_set_source_rgb(cr, 1, 1, 1);
    }

    cairo_arc(cr, current->x, current->y, NODE_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);
  }

  // selected node
  if (gs->selected.type == TYPE_NODE) {
    cairo_set_source_rgba(cr, 1, 0, 0, 0.5);
    cairo_arc(cr, gs->selected.value.node->x, gs->selected.value.node->y, NODE_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);
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
