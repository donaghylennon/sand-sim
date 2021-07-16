#include "Field.h"

Field::Field(uint32_t width, uint32_t height, uint32_t scale)
    :width(width), height(height), length(width*height), 
    field(new Block[width*height]), draw_buffer(new uint32_t[width*height]),
    scale(scale) {
    window = SDL_CreateWindow("Sand Sim", SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, width*scale, height*scale, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
    SDL_TEXTUREACCESS_STREAMING, width, height);

    for(int i = 0; i < length; i++) {
        field[i] = { air, 0, false, false };
    }

    srand(time(NULL));

}

unsigned int Field::get_index(unsigned int x, unsigned int y) {
    return y * width + x;
}

unsigned int Field::south_block(unsigned int pos) {
    unsigned int result = pos + width;
    if(result < length)
        return result;
    else
        return length;
}

unsigned int Field::southwest_block(unsigned int pos) {
    unsigned int result = pos + width - 1;
    if(result < length && result/width == pos/width + 1)
        return result;
    else
        return length;
}

unsigned int Field::southeast_block(unsigned int pos) {
    unsigned int result = pos + width + 1;
    if(result < length && result/width == pos/width + 1)
        return result;
    else
        return length;
}

unsigned int Field::west_block(unsigned int pos) {
    unsigned int result = pos - 1;
    if(result > 0 && result/width == pos/width)
        return result;
    else
        return length;
}

unsigned int Field::east_block(unsigned int pos) {
    unsigned int result = pos + 1;
    if(result < length && result/width == pos/width)
        return result;
    else
        return length;
}

unsigned int Field::northwest_block(unsigned int pos) {
    unsigned int result = pos - width - 1;
    if(result > 0 && result/width == pos/width - 1)
        return result;
    else
        return length;
}

unsigned int Field::northeast_block(unsigned int pos) {
    unsigned int result = pos - width + 1;
    if(result > 0 && result/width == pos/width - 1)
        return result;
    else
        return length;
}

unsigned int Field::north_block(unsigned int pos) {
    unsigned int result = pos - width;
    if(result > 0)
        return result;
    else
        return length;
}

inline int Field::compare_densities(unsigned int pos_a, unsigned int pos_b) {
    return materials[(int) field[pos_a].type].density > materials[(int) field[pos_b].type].density;
}

std::vector<unsigned int> Field::get_circle_points(unsigned int pos, unsigned int radius) {
    std::vector<unsigned int> points = {};
    unsigned int X_c = pos % width;
    unsigned int Y_c = pos / width;
    unsigned int X = 0;
    unsigned int Y = radius;
    int D = 3 - (int)2 * radius;

    while(X <= Y) {
        // Plot circle (X_c, Y_c, X, Y)
        for(int i = -1; i <= 1; i+=2) {
            for(int j = -1; j <= 1; j+=2) {
                //int x = X_c + (i*X);
                unsigned int pos_a = get_index(X_c + i*X, Y_c + j*Y);
                if(pos_a/width == (Y_c + j*Y))
                    points.push_back(pos_a);
                unsigned int pos_b = get_index(X_c + i*Y, Y_c + j*X);
                if(pos_b/width == (Y_c + j*X))
                    points.push_back(pos_b);
            }
        }

        //points.push_back(get_index(X_c + Y, Y_c + X));*
        //points.push_back(get_index(X_c + X, Y_c + Y));*
        //points.push_back(get_index(X_c - X, Y_c + Y));*
        //points.push_back(get_index(X_c - Y, Y_c + X));*
        //points.push_back(get_index(X_c - Y, Y_c - X));*
        //points.push_back(get_index(X_c - X, Y_c - Y));*
        //points.push_back(get_index(X_c + X, Y_c - Y));*
        //points.push_back(get_index(X_c + Y, Y_c - X));*
        
        if(D < 0) {
            D = D + 4*X + 6;
            X = X + 1;
            Y = Y;
        } else {
            D = D + 4*(X-Y) + 10;
            X = X + 1;
            Y = Y - 1;
        }
    }
    return points;
}

std::vector<unsigned int> Field::get_circle(unsigned int pos) {
    // pos - 2*width -1  pos-2*width  pos-2*width+1
    // pos - width-2 pos-width-1 pos-width pos -width+1 pos-width+2
    std::vector<unsigned int> positions = {};
    positions.push_back(pos - 2*width - 1);
    positions.push_back(pos - 2*width);
    positions.push_back(pos - 2*width + 1);
    positions.push_back(pos - width - 2);
    positions.push_back(pos - width - 1);
    positions.push_back(pos - width);
    positions.push_back(pos - width + 1);
    positions.push_back(pos - width + 2);
    positions.push_back(pos - 2);
    positions.push_back(pos - 1);
    positions.push_back(pos);
    positions.push_back(pos + 1);
    positions.push_back(pos + 2);
    positions.push_back(pos + width - 2);
    positions.push_back(pos + width - 1);
    positions.push_back(pos + width);
    positions.push_back(pos + width + 1);
    positions.push_back(pos + width + 2);
    positions.push_back(pos + 2*width - 1);
    positions.push_back(pos + 2*width);
    positions.push_back(pos + 2*width + 1);
    return positions;
}

void Field::move(unsigned int pos) {
    unsigned int next_pos;
    switch(field[pos].type) {
        case water:
            next_pos = water_next_pos(pos);
            break;
        case sand:
            next_pos = sand_next_pos(pos);
            break;
        default:
            return;
    }
    //if(next_pos >= length) {
    //    std::cout << "invalid next_pos" << std::endl;
    //    std::cout << next_pos << " and " << length << std::endl;
    //}
    //else
    //    std::cout << "valid next_pos" << std::endl;
    //Block temp = field[pos];
    //push(pos, next_pos);
    //field[next_pos] = temp;
    if(next_pos == pos) {
        field[pos].falling = false;
    } else {
        //push(pos, next_pos);
        Block temp = field[next_pos];
        field[next_pos] = field[pos];
        field[pos] = temp;
        field[pos].falling = true;
        //update_surrounding(next_pos);
        field[next_pos].updated = true;
    }
}

unsigned int Field::water_next_pos(unsigned int pos) {
    int rnd = rand() % 2;
    unsigned int south = south_block(pos);
    if(south != length && compare_densities(pos, south))
        return south;

    if(rnd) {
        unsigned int southeast = southeast_block(pos);
        if(southeast != length && compare_densities(pos, southeast))
            return southeast;

        unsigned int southwest = southwest_block(pos);
        if(southwest != length && compare_densities(pos, southwest))
            return southwest;

        unsigned int east = east_block(pos);
        if(east!= length && compare_densities(pos, east))
            return east;

        unsigned int west = west_block(pos);
        if(west != length && compare_densities(pos, west))
            return west;
    } else {
        unsigned int southwest = southwest_block(pos);
        if(southwest != length && compare_densities(pos, southwest))
            return southwest;

        unsigned int southeast = southeast_block(pos);
        if(southeast != length && compare_densities(pos, southeast))
            return southeast;

        unsigned int west = west_block(pos);
        if(west != length && compare_densities(pos, west))
            return west;

        unsigned int east = east_block(pos);
        if(east!= length && compare_densities(pos, east))
            return east;
    }

    //unsigned int southeast = southeast_block(pos);
    //if(southeast != length && compare_densities(pos, southeast))
    //    return southeast;

    //unsigned int southwest = southwest_block(pos);
    //if(southwest != length && compare_densities(pos, southwest))
    //    return southwest;

    //unsigned int east = east_block(pos);
    //if(east!= length && compare_densities(pos, east))
    //    return east;

    //unsigned int west = west_block(pos);
    //if(west != length && compare_densities(pos, west))
    //    return west;

    return pos;
}

unsigned int Field::sand_next_pos(unsigned int pos) {
    unsigned int south = south_block(pos);
    if(south != length && compare_densities(pos, south))
        return south;

    unsigned int southwest = southwest_block(pos);
    if(southwest != length && compare_densities(pos, southwest))
        return southwest;

    unsigned int southeast = southeast_block(pos);
    if(southeast != length && compare_densities(pos, southeast))
        return southeast;

    return pos;
}

void Field::push(unsigned int from_pos, unsigned int pos) {
    // Could push be implemented as simply next_pos with
    // a pushed_from position??
    unsigned int south = south_block(pos);
    if(south != length && compare_densities(pos, south) && south != from_pos) {
        field[south] = field[pos];
    }
}

void Field::update_surrounding(unsigned int pos) {
    unsigned int south = south_block(pos);
    if(south != length && materials[field[south].type].can_fall)
        field[south].falling = true;

    unsigned int southwest = southwest_block(pos);
    if(southwest != length && materials[field[southwest].type].can_fall)
        field[southwest].falling = true;

    unsigned int southeast = southeast_block(pos);
    if(southeast != length && materials[field[southeast].type].can_fall)
        field[southeast].falling = true;

    unsigned int west = west_block(pos);
    if(west != length && materials[field[west].type].can_fall)
        field[west].falling = true;

    unsigned int east = east_block(pos);
    if(east != length && materials[field[east].type].can_fall)
        field[east].falling = true;

    unsigned int northwest = northwest_block(pos);
    if(northwest != length && materials[field[northwest].type].can_fall)
        field[northwest].falling = true;

    unsigned int northeast = northeast_block(pos);
    if(northeast != length && materials[field[northeast].type].can_fall)
        field[northeast].falling = true;

    unsigned int north = north_block(pos);
    if(north != length && materials[field[north].type].can_fall)
        field[north].falling = true;
}

void Field::spawn_water(unsigned int pos) {
    for(auto& position : get_circle(pos)) {
        if(position >= 0 && position < length)
            field[position] = { water, 0, true, false };
    }
    //field[pos] = { water, 0, true, false };
    //update_surrounding(pos);
}

void Field::spawn_sand(unsigned int pos) {
    for(auto& position : get_circle(pos)) {
        if(position >= 0 && position < length)
            field[position] = { sand, 0, true, false };
    }
    //field[pos] = { sand, 0, true, false };
    //update_surrounding(pos);
}

void Field::spawn_wood(unsigned int pos) {
    for(auto& position : get_circle(pos)) {
        if(position >= 0 && position < length)
            field[position] = { wood, 0, false, false };
    }
    //field[pos] = { wood, 0, false, false };
    //update_surrounding(pos);
}

void Field::set_all_not_updated() {
    for(int i = 0; i < length; i++) {
        field[i].updated = false;
    }
}

void Field::draw() {
    for(int i = 0; i < length; i++) {
        draw_buffer[i] = materials[(int) field[i].type].colour;
    }

    draw_selector();

    SDL_UpdateTexture(texture, nullptr, draw_buffer, width*4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Field::draw_selector() {
    for(unsigned int position : get_circle_points(sel_index, sel_radius)) {
        if(position >= 0 && position < length)
            draw_buffer[position] = 0xFFFFFFFF;
    }
}

void Field::run() {
    bool running = true;
    auto prev_cycle = std::chrono::high_resolution_clock::now();
    draw();
    bool lol = true;
    bool mouse_is_down = false;
    int selection = 0;
    unsigned int x_pos = 0;
    unsigned int y_pos = 0;
    sel_radius = 5;
    while(running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
            current_time - prev_cycle
        ).count();

        if(dt > 1000/framerate) {
            prev_cycle = current_time;
            set_all_not_updated();
            for(int i = 0; i < length; i++) {
                if(!field[i].updated)
                    move(i);
            }
            draw();

            if(mouse_is_down) {
                if(selection == 0)
                    spawn_sand(sel_index);
                else if(selection == 1)
                    spawn_water(sel_index);
                else if(selection == 2)
                    spawn_wood(sel_index);
            }
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    SDL_Quit();
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouse_is_down = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouse_is_down = false;
                    break;
                case SDL_MOUSEMOTION:
                    x_pos = event.motion.x/scale;
                    y_pos = event.motion.y/scale;
                    sel_index = width*y_pos + x_pos;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_s:
                            selection = 0;
                            break;
                        case SDLK_w:
                            selection = 1;
                            break;
                        case SDLK_e:
                            selection = 2;
                            break;
                        case SDLK_3:
                            sel_radius = 3;
                            break;
                        case SDLK_4:
                            sel_radius = 4;
                            break;
                        case SDLK_5:
                            sel_radius = 5;
                            break;
                        case SDLK_6:
                            sel_radius = 6;
                            break;
                        case SDLK_7:
                            sel_radius = 7;
                            break;
                    }
                    break;
            }
        }
    }
}
