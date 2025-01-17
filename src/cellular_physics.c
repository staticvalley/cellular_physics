#define SDL_MAIN_USE_CALLBACKS 1
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <stdlib.h>

#define FRAMERATE 60
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define CELL_SIZE 16
#define CELL_WIDTH WINDOW_WIDTH / CELL_SIZE
#define CELL_HEIGHT WINDOW_HEIGHT / CELL_SIZE

typedef enum cell_type {
    CELL_EMPTY,
    CELL_BLOCK,
    CELL_SAND,
    CELL_WATER,
} cell_type;
#define NUM_CELL_TYPES 3

// application content
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static cell_type cell_grid[CELL_WIDTH][CELL_HEIGHT];
static cell_type selected_cell_type = CELL_SAND;

SDL_Point mouse_coord_to_cell(float x, float y) {

    int cell_x = x / CELL_SIZE;
    int cell_y = y / CELL_SIZE;

    return (SDL_Point) {
        .x = SDL_clamp(cell_x, 0, CELL_WIDTH),
        .y = SDL_clamp(cell_y, 0, CELL_HEIGHT),
    };
}

void draw_cell(cell_type type, int x, int y) {
    SDL_FRect cell = { x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
    switch (type) {
        case CELL_EMPTY:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            break;
        case CELL_SAND:
            SDL_SetRenderDrawColor(renderer, 200, 200, 128, SDL_ALPHA_OPAQUE);
            break;
        case CELL_BLOCK:
            SDL_SetRenderDrawColor(renderer, 178, 154, 119, SDL_ALPHA_OPAQUE);
            break;
        case CELL_WATER:
            SDL_SetRenderDrawColor(renderer, 132, 154, 214, SDL_ALPHA_OPAQUE);
            break;
    }
    SDL_RenderFillRect(renderer, &cell);
}

void draw_grid() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
    for(int i = 0; i < CELL_WIDTH; i++) {
        SDL_RenderLine(renderer, i*CELL_SIZE, 0, i*CELL_SIZE, WINDOW_HEIGHT);
    }
    for(int i = 0; i < CELL_HEIGHT; i++) {
        SDL_RenderLine(renderer, 0, i*CELL_SIZE, WINDOW_WIDTH, i*CELL_SIZE);
    }

    SDL_FRect border = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_RenderRect(renderer, &border);
}

void process_tick() {
    // draw cell types
    for (int x = CELL_WIDTH; x >= 0; x--) {
        for(int y = CELL_HEIGHT-2; y >= 0; y--) {
            if(cell_grid[x][y] == CELL_SAND) {
                if(cell_grid[x][y+1] == CELL_EMPTY) {
                    cell_grid[x][y] = CELL_EMPTY;
                    cell_grid[x][y+1] = CELL_SAND;
                }
            }
        }
    }
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

    memset(cell_grid, CELL_EMPTY, sizeof(cell_type) * CELL_WIDTH * CELL_HEIGHT);

    return SDL_APP_CONTINUE;
}

// main program loop
SDL_AppResult SDL_AppIterate(void *appstate) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // draw cell types
    for (int x = 0; x < CELL_WIDTH; x++) {
        for(int y = 0; y < CELL_HEIGHT; y++) {
            draw_cell(cell_grid[x][y], x, y);
        }
    }

    system("clear");

    // draw cell types
    for (int y = 0; y < CELL_HEIGHT; y++) {
        for(int x = 0; x < CELL_WIDTH; x++) {
            switch(cell_grid[x][y]) {
                case CELL_SAND:
                    printf("s ");
                    break;
                case CELL_EMPTY:
                    printf(". ");
                    break;
                case CELL_WATER:
                    printf("w ");
                    break;
                case CELL_BLOCK:
                    printf("b ");
                    break;
            }
        }
        printf("\n");
    }

    draw_grid();

    // process crude physics
    process_tick();

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
                SDL_Point cell_pos = mouse_coord_to_cell(event->button.x, event->button.y);
                cell_grid[cell_pos.x][cell_pos.y] = selected_cell_type;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if(event->motion.state == SDL_BUTTON_LEFT) {
                SDL_Point cell_pos = mouse_coord_to_cell(event->motion.x, event->motion.y);
                cell_grid[cell_pos.x][cell_pos.y] = selected_cell_type;
            }
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

// no cleanup needed, sdl cleans up its own entities
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
