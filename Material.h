#include <cstdint>

enum MatType {
    air, water, sand
};

class Material {
public:
    MatType type;
    uint32_t colour;
    float density;
    bool falling = true;
};

class Air : public Material {
public:
    Air() {
        type = air;
        density = 0;
        colour = 0;
    }
};

class Sand : public Material {
public:
    Sand() {
        type = sand;
        density = 0.5f;
        colour = 0x443300ff;
    }
};

class Water : public Material {
public:
    Water() {
        type = water;
        density = 0.2f;
        colour = 0x002277ff ;
    }
};
