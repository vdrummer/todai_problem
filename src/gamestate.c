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
  gs->shiftPressed = false;
  gs->moveMode = false;
  gs->numNodes = 1;
  gs->snapshotList = (SnapshotList) {0};
  memset(gs->edges, 0, MAX_NODES * MAX_NODES);
  memset(gs->nodes, 0, MAX_NODES);

  gs->nodes[0] = (Node) {
    .color = COLOR_WHITE,
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
  };
}

void snapshot_load(Gamestate* gs) {
  SnapshotList* list = &gs->snapshotList;
  if (list->length <= 0) {
    return;
  }

  list->index = (list->index - 1 + HISTSIZE) % HISTSIZE;
  memcpy(gs, list->snapshots + list->index, sizeof(Snapshot));
  list->length--;
  gs->selected.type = TYPE_NONE;
}

void snapshot_save(Gamestate* gs) {
  SnapshotList* list = &gs->snapshotList;
  memcpy(list->snapshots + list->index, gs, sizeof(Snapshot));
  list->index = (list->index + 1) % HISTSIZE;
  if (list->length < HISTSIZE) {
    list->length++;
  }
}
