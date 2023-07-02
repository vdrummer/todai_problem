#ifndef TODAIGRAPH_GAMESTATE_H
#define TODAIGRAPH_GAMESTATE_H

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

typedef enum color Color;
enum color {
  COLOR_WHITE,
  COLOR_BLACK,
  COLOR_MAXVALUE
};

typedef enum type Type;
enum type {
  TYPE_NONE,
  TYPE_NODE,
  TYPE_EDGE,
  TYPE_MAXVALUE
};

typedef struct node Node;
struct node {
  Color color;
  int x;
  int y;
};

typedef struct selection Selection;
struct selection {
  Type type;
  union {
    Node* node;
    int edge;
  } value;
};

typedef struct gamestate Gamestate;
struct gamestate {
  bool quit;
  bool shiftPressed;
  int numNodes;
  Node nodes[MAX_NODES];
  uint8_t edges[MAX_NODES * MAX_NODES];
  Selection selected;
};

void gamestate_init(Gamestate* gs);

#endif // TODAIGRAPH_GAMESTATE_H
