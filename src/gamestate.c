#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gamestate.h"
#include "constants.h"

void gamestate_init(Gamestate* gs) {
  if (gs == NULL) {
    return;
  }
  gs->quit = false;
  gs->numNodes = 1;
  memset(gs->edges, 0, MAX_NODES * MAX_NODES);
  memset(gs->nodes, 0, MAX_NODES);

  gs->nodes[0] = (Node) {
    .color = COLOR_WHITE,
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
  };
}
