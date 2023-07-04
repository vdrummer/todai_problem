# Todai Graph Problem

This is a program to visualize the graph problem from the 1998 Tokyo University entrance exam.

Problem description (Japanese): https://examist.jp/legendexam/1998-tokyo/

## Requirements (Linux)

- cmake
- make
- libsdl2
- libcairo

## Building and Running

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./todai_graph
```

## Config

You can change various parameters in `include/constants.h`.

## Usage

### Mouse Actions

- left click: select a node / edge. Deselect (if clicking anywhere else).
- shift + left click: add a node (according to the rules of the problem).
- right click (and dragging): move selected node.

### Keyboard Actions

- u: undo last action.
- b: make the selected node black.
- w: make the selected node white.

## Known Limitations

- everything is statically allocated, so everything has hardcoded limits.

## Status

I consider the program feature complete. Further improvments planned are:

- refactor rendering, gamestate
- new control scheme
- using dynamic data structures
