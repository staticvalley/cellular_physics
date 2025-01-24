#ifndef CELLULAR_PHYSICS_RENDER
#define CELLULAR_PHYSICS_RENDER

#include <SDL3/SDL.h>
#include "cell.h"
#include "config.h"

// unpack macros
#define SDL_COLOR_UNPACK(c) c.r, c.g, c.b, c.a

static SDL_Color generate_color_in_range(SDL_Color min, SDL_Color max);
SDL_Color init_cell_color(cell_type cell);
void draw_cell(SDL_Renderer* renderer, cell cell_grid[CELL_WIDTH][CELL_HEIGHT], int x, int y);
void draw_ui_grid(SDL_Renderer* renderer);
void draw_ui_block_selection(SDL_Renderer* renderer, SDL_Texture* ui_texture, cell_type selected_cell_type);
void draw_ui_brush(SDL_Renderer* renderer, SDL_Point selected_cell_index);

#endif

