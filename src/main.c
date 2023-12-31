#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cairo/cairo.h>
#include <math.h>

#include "constants.h"
#include "gamestate.h"
#include "renderstate.h"
#include "mathstuff.h"

#define GET_NODE_X(n) (n / MAX_NODES)
#define GET_NODE_Y(n) (n % MAX_NODES)
#define GET_NODE_XY(x, y) (MAX_NODES * y + x)

void selectNearest(Gamestate* gs, int x, int y) {
  // default: nothing selected
  gs->selected.type = TYPE_NONE;

  // nodes
  for (int i = 0; i < gs->graph.numNodes; i++) {
    Node* current = gs->graph.nodes + i;
    if (ms_squaredDist((Point) {x, y}, (Point) {current->x, current->y})
        < SELECTION_RADIUS * SELECTION_RADIUS
    ) {
      gs->selected.type = TYPE_NODE;
      gs->selected.value.node = current;
      return; // to prioritize nodes over edges
    }
  }

  // edges
  for (int i = 0; i < MAX_NODES * MAX_NODES; i++) {
    if (gs->graph.edges[i] > 0) {
      Node* n1 = gs->graph.nodes + GET_NODE_X(i);
      Node* n2 = gs->graph.nodes + GET_NODE_Y(i);
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
    if (gs->graph.numNodes < MAX_NODES) {
      snapshot_save(gs);
      Node* currentNode = gs->selected.value.node;
      gs->graph.nodes[gs->graph.numNodes] = (Node) {
        .color = COLOR_WHITE,
        .x = x,
        .y = y,
      };
      currentNode->color = !currentNode->color;

      // add edge
      int currentIndex = (int) (currentNode - gs->graph.nodes);
      // N.B.: only the bottom left half of the matrix is filled
      gs->graph.edges[GET_NODE_XY(currentIndex, gs->graph.numNodes)] = 1;

      gs->graph.numNodes++;
    } else {
      //TODO error handling
      perror("max number of nodes reached");
    }
  } else if (gs->selected.type == TYPE_EDGE) {
    snapshot_save(gs);
    const int selectedIdx = gs->selected.value.edge;
    const int n1Idx = GET_NODE_X(selectedIdx);
    const int n2Idx = GET_NODE_Y(selectedIdx);

    if (gs->graph.numNodes >= MAX_NODES) {
      perror("max number of nodes reached");
      return;
    }

    gs->graph.nodes[gs->graph.numNodes] = (Node) {
      .color = COLOR_WHITE,
      .x = x,
      .y = y,
    };

    gs->graph.edges[selectedIdx] = 0;
    gs->graph.edges[GET_NODE_XY(n1Idx, gs->graph.numNodes)] = 1;
    gs->graph.edges[GET_NODE_XY(n2Idx, gs->graph.numNodes)] = 1;

    gs->graph.nodes[n1Idx].color = !gs->graph.nodes[n1Idx].color;
    gs->graph.nodes[n2Idx].color = !gs->graph.nodes[n2Idx].color;

    gs->selected.type = TYPE_NONE;

    gs->graph.numNodes++;
  }
}

void activateMoveMode(Gamestate* gs) {
  if (gs->selected.type != TYPE_NODE) {
    // can only move nodes
    return;
  }

  gs->flags.moveMode = true;
  snapshot_save(gs);
}

void changeNodeColor(Gamestate* gs, Color color) {
    if (gs->selected.type != TYPE_NODE) {
        return;
    }

    if (gs->selected.value.node->color == color) {
      return;
    }

    snapshot_save(gs);
    gs->selected.value.node->color = color;
}

void moveNode(Gamestate* gs, int x, int y) {
  if (gs->flags.moveMode && gs->selected.type == TYPE_NODE) {
    Node* current = gs->selected.value.node;
    current->x = x;
    current->y = y;
  }
}

void handleEvents(Gamestate* gs) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_QUIT:
        gs->flags.quit = true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            gs->flags.shiftPressed = true;
            break;
          case SDLK_b:
            changeNodeColor(gs, COLOR_BLACK);
            break;
          case SDLK_w:
            changeNodeColor(gs, COLOR_WHITE);
            break;
          case SDLK_u:
            snapshot_load(gs);
            break;
          case SDLK_r:
            gamestate_init(gs);
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            gs->flags.shiftPressed = false;
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          if (!gs->flags.shiftPressed) {
            selectNearest(gs, event.button.x, event.button.y);
          } else {
            applyFunction(gs, event.button.x, event.button.y);
          }
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          activateMoveMode(gs);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_RIGHT) {
          gs->flags.moveMode = false;
        }
        break;
      case SDL_MOUSEMOTION:
        moveNode(gs, event.motion.x, event.motion.y);
    }
  }
}

void render(Renderstate* rs, Gamestate* gs) {
  cairo_t* cr = rs->cr;

  cairo_set_source_rgb(
      cr,
      FILLCOLOR_BACKGROUND_R,
      FILLCOLOR_BACKGROUND_G,
      FILLCOLOR_BACKGROUND_B
  );
  cairo_paint(cr);

  // render edges
  cairo_set_source_rgb(cr, 0, 0, 0);
  for (int i = 0; i < MAX_NODES * MAX_NODES; i++) {
    if (gs->graph.edges[i] != 0) {
      Node* n1 = gs->graph.nodes + GET_NODE_X(i);
      Node* n2 = gs->graph.nodes + GET_NODE_Y(i);
      cairo_set_line_width(cr, EDGE_WIDTH);
      cairo_move_to(cr, n1->x, n1->y);
      cairo_line_to(cr, n2->x, n2->y);
      cairo_close_path(cr);
      cairo_stroke(cr);
    }
  }

  // seleted edge
  if (gs->selected.type == TYPE_EDGE) {
    Node* n1 = gs->graph.nodes + GET_NODE_X(gs->selected.value.edge);
    Node* n2 = gs->graph.nodes + GET_NODE_Y(gs->selected.value.edge);
    cairo_set_source_rgba(
        cr,
        FILLCOLOR_SELECTED_R,
        FILLCOLOR_SELECTED_G,
        FILLCOLOR_SELECTED_B,
        FILLCOLOR_SELECTED_A
    );
    cairo_set_line_width(cr, EDGE_WIDTH);
    cairo_move_to(cr, n1->x, n1->y);
    cairo_line_to(cr, n2->x, n2->y);
    cairo_close_path(cr);
    cairo_stroke(cr);
  }

  // render nodes
  for (int i = 0; i < gs->graph.numNodes; i++) {
    Node* current = gs->graph.nodes + i;
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
    cairo_set_source_rgba(
        cr,
        FILLCOLOR_SELECTED_R,
        FILLCOLOR_SELECTED_G,
        FILLCOLOR_SELECTED_B,
        FILLCOLOR_SELECTED_A
    );
    cairo_arc(cr, gs->selected.value.node->x, gs->selected.value.node->y, NODE_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);
  }

  renderstate_render(rs);
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

  Renderstate rs;
  renderstate_init(&rs, r);

  while (!gs.flags.quit) {
    handleEvents(&gs);
    render(&rs, &gs);
  }

  renderstate_destroy(&rs);
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
