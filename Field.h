#include <SDL2/SDL.h>

#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <ctime>
#include <vector>

enum MatType {
    air, water, sand, wood
};

struct Material {
    unsigned int density;
    bool can_fall;
    uint32_t *colour;
};

struct Block {
    MatType type;
    unsigned int shade;
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
    uint32_t air_colour[1] = { 0 };
    uint32_t water_colour[1] = { 0x002277ff };
    uint32_t sand_colour[3] = { 0x443300ff, 0x553300ff, 0x664400ff };
    uint32_t wood_colour[2] = { 0x774400ff, 0x775500ff };
    Material materials[4] = {
        { 5, true, air_colour, }, // air
        { 9, true, water_colour, }, // water
        { 15, true, sand_colour, }, // sand
        { 40, false, wood_colour, }, // wood
    };

    uint32_t *draw_buffer;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    bool odd_turn = false;
    unsigned int sel_index = 0;
    unsigned int sel_radius = 5;

    unsigned int get_index(unsigned int x, unsigned int  y);
    unsigned int south_block(unsigned int pos);
    unsigned int southwest_block(unsigned int pos);
    unsigned int southeast_block(unsigned int pos);
    unsigned int west_block(unsigned int pos);
    unsigned int east_block(unsigned int pos);
    unsigned int northwest_block(unsigned int pos);
    unsigned int northeast_block(unsigned int pos);
    unsigned int north_block(unsigned int pos);

    inline int compare_densities(unsigned int pos_a, unsigned int pos_b);
    std::vector<unsigned int> get_circle_points(unsigned int pos, unsigned int radius);
    std::vector<unsigned int> get_circle_filled(unsigned int pos, unsigned int radius);
    std::vector<unsigned int> get_circle(unsigned int pos);
    std::vector<unsigned int> get_line_points(unsigned int src, unsigned int dst);

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
    void draw_selector();
    void run();
};
