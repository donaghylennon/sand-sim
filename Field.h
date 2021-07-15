#include <SDL2/SDL.h>

#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <ctime>

enum MatType {
    air, water, sand, wood
};

struct Material {
    unsigned int density;
    uint32_t colour;
    bool can_fall;
};

struct Block {
    MatType type;
    uint32_t shade;
    bool falling;
    bool updated;
};

enum Direction { // May be better just done as position
    NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH,
    SOUTHWEST, WEST, NORTHWEST
};

class Field {
private:
    uint32_t width;
    uint32_t height;
    uint32_t scale;
    uint32_t length;
    float framerate = 60.0f;
    Block *field;
    Material materials[4] = {
        { 5, 0, true }, // air
        { 9, 0x002277ff, true }, // water
        { 15, 0x443300ff, true }, // sand
        { 40, 0x774400ff, false }, // wood
    };

    uint32_t *draw_buffer;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    unsigned int get_index(uint32_t x, uint32_t y);
    unsigned int south_block(unsigned int pos);
    unsigned int southwest_block(unsigned int pos);
    unsigned int southeast_block(unsigned int pos);
    unsigned int west_block(unsigned int pos);
    unsigned int east_block(unsigned int pos);
    unsigned int northwest_block(unsigned int pos);
    unsigned int northeast_block(unsigned int pos);
    unsigned int north_block(unsigned int pos);

    inline int compare_densities(unsigned int pos_a, unsigned int pos_b);

public:
    Field(uint32_t width, uint32_t height, uint32_t scale);
    void move(unsigned int pos);
    unsigned int water_next_pos(unsigned int pos);
    unsigned int sand_next_pos(unsigned int pos);
    void push(unsigned int from_pos, unsigned int pos);
    void update_surrounding(unsigned int pos);

    void spawn_water(unsigned int pos);
    void spawn_sand(unsigned int pos);
    void spawn_wood(unsigned int pos);

    void set_all_not_updated();

    void draw();
    void run();
};
