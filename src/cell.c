#include "../include/cell.h"
#include "../include/render.h"

// cell grid constants
#define CELL_SIZE 16
#define CELL_WIDTH WINDOW_WIDTH / CELL_SIZE
#define CELL_HEIGHT WINDOW_HEIGHT / CELL_SIZE

void shift_down(cell cell_grid[CELL_WIDTH][CELL_HEIGHT], int x, int y) {
    if(cell_grid[x][y+1].type == CELL_EMPTY) {
        cell temp = cell_grid[x][y];
        cell_grid[x][y] = (cell) {
            CELL_EMPTY,
            cell_grid[x][y+1].fill_color,
            CELL_SIZE % 256
        };
        cell_grid[x][y+1] = temp;
    }
}

void process_tick(cell cell_grid[CELL_WIDTH][CELL_HEIGHT]) {
    for (int x = CELL_WIDTH; x >= 0; x--) {
        for(int y = CELL_HEIGHT-2; y >= 0; y--) {
            switch (cell_grid[x][y].type) {
                case CELL_WATER:
                case CELL_SAND:
                    shift_down(cell_grid, x, y);
                    break;
                default:
                    break;
            }
        }
    }
}
