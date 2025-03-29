#ifndef TILE_H
#define TILE_H

#include <set>
#include <vector>
#include "raylib.h"
#include "sprite.h"

#define ZOOM_MUTLIPLE 2  
#define CULLING_SPACE 80 

#define CHUNK_SIZE 32 
#define TILESNUM_PER_CHUNK 128
#define LOAD_RADIUS_NUM 2 //radius of chunk

#define CULLING_EDGE 150


struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

struct Tile {
    Sprite sprite;
    bool solid = false;
    Vector2 position = { 0.0f, 0.0f };
};


class TileChunk
{
public:
    int chunk_x, chunk_y;
    std::vector<int> tilesIDs;

    TileChunk() : chunk_x(0), chunk_y(0) {}
    TileChunk(int cx, int cy) : chunk_x(cx), chunk_y(cy) {}
    void load_tiles(const class TileSet& tileset);
};


class TileMap {
public:
    int width;
    int height;
    std::vector<int> TileMap_tiles;
    std::vector<TileChunk> loaded_chunks;
    std::set<std::pair<int, int>> loaded_chunk_coords;

    TileMap() : width(0), height(0) {}
    TileMap(int w, int h) : width(w), height(h), TileMap_tiles(w* h, -1) {};
    ~TileMap();

    void unload_resources();
    void randomize(const class TileSet& tileset);
    int get_tile(int x, int y) const;
    void set_tile(int x, int y, int tileid);
    bool is_chunk_loaded(int cx, int cy);
    void load_chunk(int chunk_x, int chunk_y, const TileSet& tileset);
    void render_small_tilemap(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset);
    void render_visible_tiles(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset);
    void render_visible_chunks(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset);
    void render_culling_chunks(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset, bool bculling);
    Vector2 tile_to_world(int tile_x, int tile_y);
    Vector2 world_to_tile(float world_x, float world_y);
};

class TileSet {
protected:
    std::vector<Tile> tiles;

public:
    ~TileSet();
    void add_tile(const Tile& tile);
    Tile get_tile(int id) const;
    size_t get_tile_count() const;
};

#endif  // TILE_H
