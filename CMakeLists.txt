cmake_minimum_required(VERSION 3.0)
project(todai_graph)

include_directories(include)

add_executable(
  todai_graph
  src/main.c
  src/gamestate.c
  src/mathstuff.c
  src/renderstate.c
)
target_link_libraries(todai_graph PRIVATE SDL2 cairo m)
