#ifndef TODAIGRAPH_GAMESTATE_H
#define TODAIGRAPH_GAMESTATE_H

#include <stdbool.h>
#include "constants.h"

typedef struct gamestate Gamestate;
struct gamestate {
  bool quit;
};

void gamestate_init(Gamestate* gs);

#endif // TODAIGRAPH_GAMESTATE_H
