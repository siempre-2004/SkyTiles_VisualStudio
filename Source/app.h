#ifndef APP_H
#define APP_H
#endif

#include "tile.h"
#include "raylib.h"
#include "particle.h"
#include "tilemap_loader.h"

#define LARGE_MAP_SIZE 1024
#define MEDIUM_MAP_SIZE 128
#define SMALL_MAP_SIZE 32

#define TILE_SIZE 16
#define MAP_MOVE_SPEED 64
#define CAMERA_ZOOM 1.5
#define MAP_MATERIAl_WIDTH 11
#define MAP_MATERIAl_HEIGHT 12

template<typename T> //Limiting a value to a specified minimum and maximum value
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}


class App {
    Camera2D camera;
    Texture2D spritesheet;
    Texture2D background;
    TileSet Apptileset;
    bool culling_enabled;
    bool perform_culling;
    TileMap small_tilemap;
    TileMap medium_tilemap;
    TileMap large_tilemap;
    TileMap tilemap_load;
    TileMap* current_tilemap;
    int current_level;

public:
    App()
        : camera{ 0 },
        spritesheet{},
        Apptileset{},
        culling_enabled(false),
        perform_culling(false),
        small_tilemap(SMALL_MAP_SIZE, SMALL_MAP_SIZE),
        medium_tilemap(MEDIUM_MAP_SIZE, MEDIUM_MAP_SIZE),
        large_tilemap(LARGE_MAP_SIZE, LARGE_MAP_SIZE),
        current_tilemap(&small_tilemap),
        current_level(0),
        map_center_x(0),
        map_center_y(0) {
    }
    ~App();

    void init();
    void update();
    void render();

private:
    int tile_width = TILE_SIZE;
    int tile_height = TILE_SIZE;
    float map_center_x;
    float map_center_y;
};