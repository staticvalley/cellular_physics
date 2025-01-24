#ifndef CELLULAR_PHYSICS_CELL
#define CELLULAR_PHYSICS_CELL

#include <SDL3/SDL.h>
#include "config.h"

typedef enum cell_type {
    CELL_EMPTY,
    CELL_BLOCK,
    CELL_SAND,
    CELL_WATER, 
} cell_type;
#define NUM_CELL_TYPES 4

typedef struct cell {
    cell_type type;
    SDL_Color fill_color;
    float fill_percent;
} cell;

void shift_down(cell cell_grid[CELL_WIDTH][CELL_HEIGHT], int x, int y);
void process_tick(cell cell_grid[CELL_WIDTH][CELL_HEIGHT]);

#endif
