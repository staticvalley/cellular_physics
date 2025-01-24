#define SDL_MAIN_USE_CALLBACKS 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>

#include "../include/render.h"
#include "../include/cell.h"

// application content
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static cell cell_grid[CELL_WIDTH][CELL_HEIGHT];
static SDL_Point mouse_coord = { 0, 0 };
static cell_type selected_cell_type = CELL_SAND;
static SDL_Texture* selection_ui_spritemap = NULL;

void update_global_mouse_coord(float x, float y) {
    int cell_x = x / CELL_SIZE;
    int cell_y = y / CELL_SIZE;
    mouse_coord.x = SDL_clamp(cell_x, 0, CELL_WIDTH);
    mouse_coord.y = SDL_clamp(cell_y, 0, CELL_HEIGHT);
}

// program initialization
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("sdl3 video subsystem could not be initialized: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_Init(SDL_INIT_EVENTS)) { 
        SDL_Log("sdl3 event subsystem could not be initialized: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("basic cellular physics sim ^-^", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("sdl3 window/renderer pair could not be initialized: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    srand(time(NULL));

    // fill grid with empty cells
    const cell init_cell = { CELL_EMPTY, CELL_SIZE % 256, 1.0};
    for (int x = 0; x < CELL_WIDTH; x++)
        for(int y = 0; y < CELL_HEIGHT; y++)
           cell_grid[x][y] = init_cell;

    // load ui bmps to textures
    SDL_Surface* selection_ui_surface = SDL_LoadBMP("../assets/selection_ui_sprite_sheet.bmp");
    if(!selection_ui_surface) {
        SDL_Log("sdl3 could not load .bmp file to surface: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    selection_ui_spritemap = SDL_CreateTextureFromSurface(renderer, selection_ui_surface);
    if(!selection_ui_spritemap) { 
        SDL_Log("sdl3 could not load surface to texture: %s", SDL_GetError());
        SDL_DestroySurface(selection_ui_surface);
        return SDL_APP_FAILURE;
    }
    SDL_DestroySurface(selection_ui_surface);

    return SDL_APP_CONTINUE;
}

// main program loop
SDL_AppResult SDL_AppIterate(void *appstate) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // draw cell types
    for (int x = 0; x < CELL_WIDTH; x++) {
        for(int y = 0; y < CELL_HEIGHT; y++) {
            draw_cell(renderer, cell_grid, x, y);
        }
    }

    // draw ui
    draw_ui_grid(renderer);
    draw_ui_block_selection(renderer, selection_ui_spritemap, selected_cell_type);
    draw_ui_brush(renderer, mouse_coord);

    // process crude physics
    process_tick(cell_grid);

    // run at set framerate
    SDL_Delay(1000/FRAMERATE);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}


// event handler
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                cell_grid[mouse_coord.x][mouse_coord.y] = (cell) {
                    selected_cell_type,
                    init_cell_color(selected_cell_type),
                    1.0
                };
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            update_global_mouse_coord(event->motion.x, event->motion.y);
            if(event->motion.state == SDL_BUTTON_LEFT) {
                cell_grid[mouse_coord.x][mouse_coord.y] = (cell) {
                    selected_cell_type,
                    init_cell_color(selected_cell_type),
                    1.0
                };
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if(event->wheel.y > 0) {
                // scrolled forward
                selected_cell_type = (selected_cell_type + 1) % NUM_CELL_TYPES;
            } else { 
                // scrolled backward
                selected_cell_type = (selected_cell_type - 1);
                selected_cell_type = selected_cell_type == -1 ? NUM_CELL_TYPES - 1 : selected_cell_type;
            }
            //printf("selected cell type: %d\n", selected_cell_type);
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

// no cleanup needed, sdl cleans up its own entities
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
