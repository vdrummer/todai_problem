#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gamestate.h"

void gamestate_init(Gamestate* gs) {
  if (gs == NULL) {
    return;
  }
  gs->quit = false;
  gs->numNodes = 0;
  memset(gs->edges, 0, MAX_NODES * MAX_NODES);
  memset(gs->nodes, 0, MAX_NODES);
}
