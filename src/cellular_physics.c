#define SDL_MAIN_USE_CALLBACKS 1

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
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
#include <time.h>

#define SDL_COLOR_EXPAND(c) c.r, c.g, c.b, c.a

// render constants
#define FRAMERATE 60
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// cell grid constants
#define CELL_SIZE 16
#define CELL_WIDTH WINDOW_WIDTH / CELL_SIZE
#define CELL_HEIGHT WINDOW_HEIGHT / CELL_SIZE

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
    uint8_t fill_percent;
} cell;

// application content
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static cell cell_grid[CELL_WIDTH][CELL_HEIGHT];
static SDL_Point selected_cell = { 0, 0 };
static cell_type selected_cell_type = CELL_SAND;

static SDL_Texture* test_texture;

void update_global_mouse_coord(float x, float y);
void draw_cell(int x, int y);
SDL_Color get_cell_color(int x, int y);
void draw_grid();
void draw_brush();
void shift_down(int x, int y);
void process_tick();

void update_global_mouse_coord(float x, float y) {
    int cell_x = x / CELL_SIZE;
    int cell_y = y / CELL_SIZE;
    selected_cell.x = SDL_clamp(cell_x, 0, CELL_WIDTH);
    selected_cell.y = SDL_clamp(cell_y, 0, CELL_HEIGHT);
}

SDL_Color vary_cell_coloring(SDL_Color min, SDL_Color max) {
    return (SDL_Color) {
        .r = min.r + rand() % (max.r - min.r + 1),
        .g = min.g + rand() % (max.g - min.g + 1),
        .b = min.b + rand() % (max.b - min.b + 1),
        .a = SDL_ALPHA_OPAQUE
    };
}

void update_cell(cell_type type, uint8_t fill_percent, int x, int y) {
    cell_grid[x][y].type = type;
    cell_grid[x][y].fill_percent = fill_percent;
    cell_grid[x][y].fill_color = get_cell_color(x, y);
}

SDL_Color get_cell_color(int x, int y) {
    SDL_Color cell_color = { 0, 0, 0, SDL_ALPHA_OPAQUE };
    switch (cell_grid[x][y].type) {
        case CELL_EMPTY:
            break;
        case CELL_SAND:
            cell_color = vary_cell_coloring(
                (SDL_Color){200, 200, 101, SDL_ALPHA_OPAQUE},
                (SDL_Color){230, 230, 128, SDL_ALPHA_OPAQUE}
            );
            break;
        case CELL_BLOCK:
            cell_color = (SDL_Color){178, 154, 119};
            break;
        case CELL_WATER:
            cell_color = vary_cell_coloring(
                (SDL_Color){132, 154, 214, SDL_ALPHA_OPAQUE}, 
                (SDL_Color){100, 130, 255, SDL_ALPHA_OPAQUE}
            );
            printf("%d, %d, %d\n", SDL_COLOR_EXPAND(cell_color));
            break;
    }
    return cell_color;
}

void draw_cell(int x, int y) {
    SDL_FRect cell = { x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(renderer, SDL_COLOR_EXPAND(cell_grid[x][y].fill_color));
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

void draw_block_selection_ui() {
    const SDL_FRect ui_box = { (float)CELL_SIZE / 2, (float)CELL_SIZE / 2, test_texture->w / 2, test_texture->h / 2 };
    SDL_FRect selection_sprite = { 0, 0, (float)test_texture->w / 2, (float)test_texture->h / 2 };
    switch (selected_cell_type) {
        case CELL_EMPTY:
            selection_sprite.x = (float)test_texture->w / 2;
            selection_sprite.y = (float)test_texture->h / 2;
            SDL_RenderTexture(renderer, test_texture, &selection_sprite, &ui_box);
            break;
        case CELL_SAND:
            selection_sprite.x = 0;
            selection_sprite.y = (float)test_texture->h / 2;
            SDL_RenderTexture(renderer, test_texture, &selection_sprite, &ui_box);
            break;
        case CELL_BLOCK:
            selection_sprite.x = 0;
            selection_sprite.y = 0;
            SDL_RenderTexture(renderer, test_texture, &selection_sprite, &ui_box);
            break;
        case CELL_WATER:
            selection_sprite.x = (float)test_texture->w / 2;
            selection_sprite.y = 0;
            SDL_RenderTexture(renderer, test_texture, &selection_sprite, &ui_box);
            break;
    }
}

void draw_brush() {
    SDL_SetRenderDrawColor(renderer, 64, 188, 78, SDL_ALPHA_OPAQUE);
    SDL_FRect border = { selected_cell.x * CELL_SIZE, selected_cell.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_RenderRect(renderer, &border);
}

void shift_down(int x, int y) {
    if(cell_grid[x][y+1].type == CELL_EMPTY) {
        cell current_cell = cell_grid[x][y];
        update_cell(CELL_EMPTY, CELL_SIZE % 256, x, y);
        update_cell(current_cell.type, current_cell.fill_percent, x, y + 1);
    }
}

void diffuse_cell(int x, int y) {
    const float max_compression = 0.2;

}

void process_tick() {
    // draw cell types
    for (int x = CELL_WIDTH; x >= 0; x--) {
        for(int y = CELL_HEIGHT-2; y >= 0; y--) {
            switch (cell_grid[x][y].type) {
                case CELL_WATER:
                case CELL_SAND:
                    shift_down(x, y);
                    break;
                default:
                    break;
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

    srand(time(NULL));

    const cell init_cell = { CELL_EMPTY, CELL_SIZE % 256 };
    for (int x = 0; x < CELL_WIDTH; x++)
        for(int y = 0; y < CELL_HEIGHT; y++)
           cell_grid[x][y] = init_cell;

    SDL_Surface* test_surface = SDL_LoadBMP("../assets/selection_ui_sprite_sheet.bmp");
    if(!test_surface) { puts("broke"); } 
    test_texture = SDL_CreateTextureFromSurface(renderer, test_surface);
    if(!test_texture) { puts("broke"); } 
    SDL_DestroySurface(test_surface);

    return SDL_APP_CONTINUE;
}


// main program loop
SDL_AppResult SDL_AppIterate(void *appstate) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // draw cell types
    for (int x = 0; x < CELL_WIDTH; x++) {
        for(int y = 0; y < CELL_HEIGHT; y++) {
            draw_cell(x, y);
        }
    }

    // draw ui
    draw_grid();
    draw_block_selection_ui();
    draw_brush();

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
                update_global_mouse_coord(event->button.x, event->button.y);
                update_cell(selected_cell_type, CELL_SIZE % 256, selected_cell.x, selected_cell.y);
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            update_global_mouse_coord(event->motion.x, event->motion.y);
            if(event->motion.state == SDL_BUTTON_LEFT) {
                update_cell(selected_cell_type, CELL_SIZE % 256, selected_cell.x, selected_cell.y);
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
