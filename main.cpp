#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#define NROWS 300
#define NCOLS 300
#define SCALE 3
#define FRAMERATE 60

enum mat_type {
    air, water, sand
};

struct material {
    mat_type type = air;
    uint32_t colour = 0;
    bool moved_this_tick = false;
};

void phys_tick(material *field);
void phys_water(material *field, unsigned int index);
void phys_sand(material *field, unsigned int index);
void set_all_unmoved(material *field);
void update_buffer(material *field, uint32_t *buffer);
void draw(SDL_Renderer *renderer, SDL_Texture *texture, uint32_t *buffer);

int main() {
    material field[NROWS*NCOLS] = {};
    uint32_t draw_buffer[NROWS*NCOLS] = {};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Sand Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, NROWS*SCALE, NCOLS*SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, NCOLS, NROWS);

    auto prev_cycle = std::chrono::high_resolution_clock::now();
    bool running = true;
    draw(renderer, texture, draw_buffer);
    while(running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - prev_cycle).count();

        if(dt > 1000/FRAMERATE) {
            field[NCOLS/2] = { sand, 0x443300ff };
            //field[NCOLS/2 + 20] = { water, 0x002277ff };
            //field[0] = { sand, 0x443300ff };
            field[NCOLS-1] = { water, 0x002277ff };
            prev_cycle = current_time;
            phys_tick(field);
            update_buffer(field, draw_buffer);
            draw(renderer, texture, draw_buffer);
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    SDL_Quit();
                    break;
            }
        }
    }
}

void phys_tick(material *field) {
    set_all_unmoved(field);
    for(int i = 0; i < NROWS*NCOLS; i++) {
        if(field[i].moved_this_tick)
            continue;
        switch(field[i].type) {
            case water:
                phys_water(field, i);
                break;
            case sand:
                phys_sand(field, i);
                break;
        }
        field[i].moved_this_tick = true; // Maybe move into phys functions to ignore air
    }
}

void phys_water(material *field, unsigned int index) {
    if(index + NCOLS < NROWS*NCOLS && field[index + NCOLS].type == air) {
        field[index + NCOLS] = field[index];
        field[index] = { air, 0 };
    } else if(index + NCOLS-1 < NROWS*NCOLS
            && (index + NCOLS-1)/NCOLS == index/NCOLS + 1 // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS-1].type == air) {
        field[index + NCOLS-1] = field[index];
        field[index] = { air, 0 };
    } else if(index + NCOLS+1 < NROWS*NCOLS
            && (index + NCOLS+1)/NCOLS == index/NCOLS + 1 // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS+1].type == air) {
        field[index + NCOLS+1] = field[index];
        field[index] = { air, 0 };
    } else if(index > 0
            && (index - 1)/NCOLS == index/NCOLS // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index - 1].type == air) {
        field[index - 1] = field[index];
        field[index] = { air, 0 };
    } else if(index < NROWS*NCOLS-1 
            && (index + 1)/NCOLS == index/NCOLS // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + 1].type == air) {
        field[index + 1] = field[index];
        field[index] = { air, 0 };
    }
}

void phys_sand(material *field, unsigned int index) {
    if(index + NCOLS < NROWS*NCOLS && field[index + NCOLS].type == air) {
        field[index + NCOLS] = field[index];
        field[index] = { air, 0 };
    } else if(index + NCOLS-1 < NROWS*NCOLS 
            && (index + NCOLS-1)/NCOLS == index/NCOLS + 1 // Ensures sand at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS-1].type == air) {
        field[index + NCOLS-1] = field[index];
        field[index] = { air, 0 };
    } else if(index + NCOLS+1 < NROWS*NCOLS
            && (index + NCOLS+1)/NCOLS == index/NCOLS + 1 // Ensures sand at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS+1].type == air) {
        field[index + NCOLS+1] = field[index];
        field[index] = { air, 0 };
    }
}

void set_all_unmoved(material *field) {
    for(int i = 0; i < NROWS*NCOLS; i++) {
        field[i].moved_this_tick = false;
    }
}

void update_buffer(material *field, uint32_t *buffer) {
    for(int i = 0; i < NROWS*NCOLS; i++) {
        buffer[i] = field[i].colour;
    }
}

void draw(SDL_Renderer *renderer, SDL_Texture *texture, uint32_t *buffer) {
    SDL_UpdateTexture(texture, nullptr, buffer, NCOLS*4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
