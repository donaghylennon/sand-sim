#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#include "Material.h"

#define NROWS 150
#define NCOLS 150
#define SCALE 6
#define FRAMERATE 60

//enum mat_type {
//    air, water, sand
//};
//
//struct MatType {
//    float density = 0.0f;
//
//    static MatType air;
//    static MatType sand;
//    static MatType water;
//    constexpr bool operator==(const MatType& other) {
//        return density==other.density;
//    }
//};

//MatType MatType::air {
//    0.0f
//};
//MatType MatType::sand {
//    0.7f
//};
//MatType MatType::water {
//    0.2f
//};

//struct material {
//    MatType type = MatType::air;
//    uint32_t colour = 0;
//    bool moved_this_tick = false;
//
//    material() {}
//    material(MatType t, uint32_t c): type(t), colour(c) {}
//    constexpr material& operator=(const material& other) {
//        type = other.type;
//        colour = other.colour;
//        moved_this_tick = other.moved_this_tick;
//        return *this;
//    }
//    constexpr bool operator==(const material& other) {
//        if(type == other.type && colour == other.colour)
//            return true;
//        else
//            return false;
//    }
//};

void phys_tick(Material *field);
void phys_water(Material *field, unsigned int index);
void phys_sand(Material *field, unsigned int index);
//void set_all_unmoved(Material *field);
void update_buffer(Material *field, uint32_t *buffer);
void draw(SDL_Renderer *renderer, SDL_Texture *texture, uint32_t *buffer);

int main() {
    Material field[NROWS*NCOLS] = { Air() };
    uint32_t draw_buffer[NROWS*NCOLS] = {};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Sand Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, NROWS*SCALE, NCOLS*SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, NCOLS, NROWS);

    auto prev_cycle = std::chrono::high_resolution_clock::now();
    bool running = true;
    draw(renderer, texture, draw_buffer);
    bool lol = true;
    while(running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - prev_cycle).count();

        if(dt > 1000/FRAMERATE) {
            //field[NCOLS/2] = material( MatType::sand, 0x443300ff );
            if(lol)
                field[NCOLS/2] = Sand();
            //field[NCOLS/2 + 20] = { water, 0x002277ff };
            //field[0] = { sand, 0x443300ff };
            if(!lol)
                field[NCOLS-1] = Water();
            lol = !lol;
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

void phys_tick(Material *field) {
    //set_all_unmoved(field);
    for(int i = 0; i < NROWS*NCOLS; i++) {
        if(!field[i].falling)
            continue;
        if(field[i].type == MatType::water)
            phys_water(field, i);
        else if(field[i].type == MatType::sand)
            phys_sand(field, i);
        //switch(field[i].type) {
        //    case MatType::water:
        //        phys_water(field, i);
        //        break;
        //    case MatType::sand:
        //        phys_sand(field, i);
        //        break;
        //}
        //field[i].moved_this_tick = true; // Maybe move into phys functions to ignore air
    }
}

void phys_water(Material *field, unsigned int index) {
    if(index + NCOLS < NROWS*NCOLS && field[index + NCOLS].density < field[index].density) {
        auto temp = field[index + NCOLS];
        field[index + NCOLS] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index + NCOLS-1 < NROWS*NCOLS
            && (index + NCOLS-1)/NCOLS == index/NCOLS + 1 // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS-1].density < field[index].density) {
        auto temp = field[index + NCOLS-1];
        field[index + NCOLS-1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index + NCOLS+1 < NROWS*NCOLS
            && (index + NCOLS+1)/NCOLS == index/NCOLS + 1 // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS+1].density < field[index].density) {
        auto temp = field[index + NCOLS+1];
        field[index + NCOLS+1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index > 0
            && (index - 1)/NCOLS == index/NCOLS // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index - 1].density < field[index].density) {
        auto temp = field[index - 1];
        field[index - 1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index < NROWS*NCOLS-1 
            && (index + 1)/NCOLS == index/NCOLS // Ensures water at screen edges doesn't wrap around to other side of screen
            && field[index + 1].density < field[index].density) {
        auto temp = field[index + 1];
        field[index + 1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else {
        field[index].falling = false;
    }
}

void phys_sand(Material *field, unsigned int index) {
    if(index + NCOLS < NROWS*NCOLS && field[index + NCOLS].density < field[index].density) {
        auto temp = field[index + NCOLS];
        field[index + NCOLS] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index + NCOLS-1 < NROWS*NCOLS 
            && (index + NCOLS-1)/NCOLS == index/NCOLS + 1 // Ensures sand at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS-1].density < field[index].density) {
        auto temp = field[index + NCOLS-1];
        field[index + NCOLS-1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else if(index + NCOLS+1 < NROWS*NCOLS
            && (index + NCOLS+1)/NCOLS == index/NCOLS + 1 // Ensures sand at screen edges doesn't wrap around to other side of screen
            && field[index + NCOLS+1].density < field[index].density) {
        auto temp = field[index + NCOLS+1];
        field[index + NCOLS+1] = field[index];
        field[index] = temp;
        if(field[index].density > 0.0f)
            field[index].falling = true;
    } else {
        field[index].falling = false;
    }
}

//void set_all_unmoved(Material *field) {
//    for(int i = 0; i < NROWS*NCOLS; i++) {
//        field[i].moved_this_tick = false;
//    }
//}

void update_buffer(Material *field, uint32_t *buffer) {
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
