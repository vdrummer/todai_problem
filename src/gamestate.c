#include <stdbool.h>
#include <stdlib.h>

#include "gamestate.h"

void gamestate_init(Gamestate* gs) {
  if (gs == NULL) {
    return;
  }
  gs->quit = false;
}
