#include "../include/render.h"
#include "../include/cell.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>
#include <stdlib.h>

static SDL_Color generate_color_in_range(SDL_Color min, SDL_Color max) {
    return (SDL_Color) {
        .r = min.r + rand() % (max.r - min.r + 1),
        .g = min.g + rand() % (max.g - min.g + 1),
        .b = min.b + rand() % (max.b - min.b + 1),
        .a = SDL_ALPHA_OPAQUE
    };
}

SDL_Color init_cell_color(cell_type cell) {
    SDL_Color cell_color = { 0, 0, 0, SDL_ALPHA_OPAQUE };
    switch (cell) {
        case CELL_EMPTY:
            break;
        case CELL_SAND:
            cell_color = generate_color_in_range(
                (SDL_Color){200, 200, 101, SDL_ALPHA_OPAQUE},
                (SDL_Color){230, 230, 128, SDL_ALPHA_OPAQUE}
            );
            break;
        case CELL_BLOCK:
            cell_color = (SDL_Color){178, 154, 119};
            break;
        case CELL_WATER:
            cell_color = generate_color_in_range(
                (SDL_Color){132, 154, 214, SDL_ALPHA_OPAQUE}, 
                (SDL_Color){100, 130, 255, SDL_ALPHA_OPAQUE}
            );
            break;
    }
    return cell_color;
}

void draw_cell(SDL_Renderer* renderer, cell cell_grid[CELL_WIDTH][CELL_HEIGHT], int x, int y) {
    SDL_FRect cell = { x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(renderer, SDL_COLOR_UNPACK(cell_grid[x][y].fill_color));
    SDL_RenderFillRect(renderer, &cell);
}

void draw_ui_grid(SDL_Renderer* renderer) {
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

void draw_ui_block_selection(SDL_Renderer* renderer, SDL_Texture* ui_texture, cell_type selected_cell_type) {
    const SDL_FRect ui_box = { 
        (float)CELL_SIZE / 2, 
        (float)CELL_SIZE / 2, 
        (float)ui_texture->w / 2, 
        (float)ui_texture->h / 2 
    };
    SDL_FRect selection_sprite = { 
        0, 
        0, 
        (float)ui_texture->w / 2, 
        (float)ui_texture->h / 2 
    };
    // shift texture window to correct sprite
    switch (selected_cell_type) {
        case CELL_EMPTY:
            selection_sprite.x = (float)ui_texture->w / 2;
            selection_sprite.y = (float)ui_texture->h / 2;
            SDL_RenderTexture(renderer, ui_texture, &selection_sprite, &ui_box);
            break;
        case CELL_SAND:
            selection_sprite.x = 0;
            selection_sprite.y = (float)ui_texture->h / 2;
            SDL_RenderTexture(renderer, ui_texture, &selection_sprite, &ui_box);
            break;
        case CELL_BLOCK:
            selection_sprite.x = 0;
            selection_sprite.y = 0;
            SDL_RenderTexture(renderer, ui_texture, &selection_sprite, &ui_box);
            break;
        case CELL_WATER:
            selection_sprite.x = (float)ui_texture->w / 2;
            selection_sprite.y = 0;
            SDL_RenderTexture(renderer, ui_texture, &selection_sprite, &ui_box);
            break;
    }
}

void draw_ui_brush(SDL_Renderer* renderer, SDL_Point selected_cell_index) {
    SDL_SetRenderDrawColor(renderer, 64, 188, 78, SDL_ALPHA_OPAQUE);
    SDL_FRect border = { selected_cell_index.x * CELL_SIZE, selected_cell_index.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_RenderRect(renderer, &border);
}
