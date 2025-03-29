#include "app.h"
#include "raylib.h"
#include <vector>
#include "particle.h"
#include <iostream>

constexpr int min_offset_x = 50;
constexpr int max_offset_x = 800;
constexpr int min_offset_y = 50;
constexpr int max_offset_y = 800;

float Lerp(float start, float end, float amount) {
    return (1.0f - amount) * start + amount * end;
}

float Damp(float source, float target, float lambda, float deltaTime) {
    return Lerp(source, target, 1 - expf(-lambda * deltaTime));
}

std::vector<Particle> particles;

void App::init()
{
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(1080);

    camera.zoom = (float)CAMERA_ZOOM;

    map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    camera.target = { map_center_x, map_center_y };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    spritesheet = LoadTexture("Source/tilesheet_packed.png");
    background = LoadTexture("Source/sky.png");
    tilemap_load = loadTileMapFromJSON("Source/tilemap.json");

    for (int y = 0; y < MAP_MATERIAl_WIDTH; y++)
    { //cut the spritesheet and assign them
        for (int x = 0; x < MAP_MATERIAl_HEIGHT; x++)
        {
            Tile tile = {};
            tile.sprite = get_sprite_from_sheet(x, y, tile_width, tile_height);
            tile.solid = false;
            Apptileset.add_tile(tile);
        }
    }
    current_tilemap->randomize(Apptileset);
}

void App::update()
{
    static Vector2 camera_velocity = { 0.0f, 0.0f }; // camera speed
    map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;

    Vector2 move_direction = {};
    static Vector2 target_position = camera.target;// smooth the target position of the follow
    float move_speed = 500.0f;
    float damping_factor = 3.0f;

    if (IsKeyDown(KEY_LEFT)) target_position.x -= move_speed * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) target_position.x += move_speed * GetFrameTime();
    if (IsKeyDown(KEY_UP)) target_position.y -= move_speed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) target_position.y += move_speed * GetFrameTime();

    camera.target.x = Damp(camera.target.x, target_position.x, damping_factor, GetFrameTime());
    camera.target.y = Damp(camera.target.y, target_position.y, damping_factor, GetFrameTime());

    static bool boundary_limit_enabled = false;
    static bool boundary_limit_init = false;
    static Vector2 old_camera_target = camera.target;

    if (!boundary_limit_init) {
        old_camera_target = camera.target;
        boundary_limit_init = !boundary_limit_init;
    }

    if (IsKeyPressed(KEY_C) && current_tilemap == &small_tilemap)
    {
        Vector2 vtarget = old_camera_target;
        old_camera_target = camera.target;

        boundary_limit_enabled = !boundary_limit_enabled;

        if (boundary_limit_enabled) {
            camera.target.x = (float)((current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / (2.0f));
            camera.target.y = (float)((current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / (2.0f));
        }
        else {
            camera.target = vtarget;
        }
        target_position = camera.target;
    }

    if (boundary_limit_enabled)
    {
        float min_x = 0, min_y = 0;
        float max_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        float max_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        float offset_x = (GetScreenWidth() / camera.zoom - (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE)) / 2.0f;
        float offset_y = (GetScreenHeight() / camera.zoom - (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE)) / 2.0f;
        if (offset_x >= 0) {
            min_x = max_x;
        }
        else {
            min_x = max_x + offset_x; // making the camera target back to center
            max_x = max_x - offset_x;
        }

        if (offset_y >= 0) {
            min_y = max_y;
        }
        else {
            min_y = max_y + offset_y;
            max_y = max_y - offset_y;
        }
        camera.target.x = Clamp(camera.target.x, min_x, max_x);
        camera.target.y = Clamp(camera.target.y, min_y, max_y);
    }

    if (IsKeyPressed(KEY_F)) {
        culling_enabled = !culling_enabled;
        if (!culling_enabled) {
            perform_culling = false;
        }
    }

    if (IsKeyPressed(KEY_G)) {
        if (culling_enabled) {
            perform_culling = !perform_culling;
        }
        else {}
    }

    if (IsKeyPressed(KEY_SPACE)) {
        current_tilemap->randomize(Apptileset);
    }

    //using space key to change from smalltilemap to bigger one
    if (IsKeyPressed(KEY_TAB)) {
        if (current_tilemap == &small_tilemap) {
            current_tilemap = &large_tilemap;
        }
        else {
            current_tilemap = &small_tilemap;
        }

        map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        camera.target = { map_center_x, map_center_y };
        target_position = camera.target;
    }

    if (IsKeyPressed(KEY_R)) { //Pressing R key to load the 128*128 map
        medium_tilemap = loadTileMapFromJSON("Source/tilemap.json");
        current_tilemap = &medium_tilemap;
        map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        camera.target = { map_center_x, map_center_y };
        target_position = camera.target;
    }

    Vector2 particle_position = { (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f ,
        (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f };
    spawn_particle({ (float)(rand() % 50 - 25), (float)(rand() % 50 - 25) }, 70.0f);//Velocity(100,50), life_time 5 seconds
    update_particles(GetFrameTime());
}

void App::render()
{
    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    int bg_width = background.width * 2;
    int bg_height = background.height * 2;
    float cam_x = camera.target.x;
    float cam_y = camera.target.y;
    //background image scrolls according to the movement of the camera position 
    float bg_offset_x = cam_x - fmodf(cam_x, (float)bg_width);
    float bg_offset_y = cam_y - fmodf(cam_y, (float)bg_height);

    for (int y = -1; y <= 1; y++)//3x3 grid traversal
    {
        for (int x = -1; x <= 1; x++)
        {
            Vector2 bg_position = { bg_offset_x + x * bg_width, bg_offset_y + y * bg_height };
            DrawTextureEx(background, bg_position, 0.0f, 2.0f, WHITE);
        }
    }

    if (culling_enabled) {
        current_tilemap->render_culling_chunks(spritesheet, camera, Apptileset, perform_culling); //mod202501
    }
    else {
        current_tilemap->render_visible_chunks(spritesheet, camera, Apptileset);
    }

    render_particles(camera);
    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
}

App::~App() {
    UnloadTexture(spritesheet);
    UnloadTexture(background);
    particles.clear();
}

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "zhehan_hu_render_assignment");

    App app;
    app.init();

    while (!WindowShouldClose()) {
        app.update();
        app.render();
    }

    CloseWindow();
    return 0;
}