#include "tile.h"
#include <cstdlib>
#include <iostream>
#include "raylib.h"
#include <unordered_set>
#include "app.h"
#include "tilemap_loader.h"


Vector2 TileMap::tile_to_world(int tile_x, int tile_y) {
    return { (float)(tile_x * TILE_SIZE * ZOOM_MUTLIPLE), (float)(tile_y * TILE_SIZE * ZOOM_MUTLIPLE) }; //mod
}
Vector2 TileMap::world_to_tile(float world_x, float world_y) {
    return { world_x / (float)(TILE_SIZE * ZOOM_MUTLIPLE), world_y / (float)(TILE_SIZE * ZOOM_MUTLIPLE) }; //mod
}

TileMap small_tilemap(SMALL_MAP_SIZE, SMALL_MAP_SIZE);
TileMap large_tilemap(LARGE_MAP_SIZE, LARGE_MAP_SIZE);
TileMap* current_tilemap = &small_tilemap;
//Randomly assign IDs to TileMaps and Tiles within Chunk
void TileMap::randomize(const TileSet& tileset) {
    size_t tile_count = tileset.get_tile_count();
    for (auto& tileid : TileMap_tiles) {
        tileid = rand() % tile_count;
    }
    for (auto& chunk : loaded_chunks) {
        for (auto& tileid : chunk.tilesIDs) {
            tileid = rand() % tile_count;
        }
    }
}

int TileMap::get_tile(int x, int y) const {
    int index = y * width + x;
    return TileMap_tiles[index];
}
//Convert 2D coordinates to 1D indexes
void TileMap::set_tile(int x, int y, int tileid) {
    int index = y * width + x;
    TileMap_tiles[index] = tileid;
}

void TileSet::add_tile(const Tile& tile) {
    tiles.push_back(tile);
}


Tile TileSet::get_tile(int index) const {
    if (index >= 0 && index < (int)tiles.size()) {
        return tiles[index];
    }
    return Tile();
}

size_t TileSet::get_tile_count() const {
    return tiles.size();
}

void TileChunk::load_tiles(const TileSet& tileset) {
    int tiles_per_chunk = CHUNK_SIZE * CHUNK_SIZE;
    tilesIDs.resize(tiles_per_chunk); // tilesIDs are resized to tiles_per_chunk size
    for (int i = 0; i < tiles_per_chunk; ++i) {
        tilesIDs[i] = rand() % tileset.get_tile_count();
    }
}

bool TileMap::is_chunk_loaded(int cx, int cy) {
    for (const auto& chunk : loaded_chunks) {
        if (chunk.chunk_x == cx && chunk.chunk_y == cy) {
            return true;
        }
    }
    return false;
}

void TileMap::load_chunk(int chunk_x, int chunk_y, const TileSet& tileset) {
    if (!is_chunk_loaded(chunk_x, chunk_y)) {
        TileChunk chunk(chunk_x, chunk_y);
        chunk.load_tiles(tileset);
        loaded_chunks.push_back(chunk);
        loaded_chunk_coords.insert({ chunk_x, chunk_y });
    }
}

void TileMap::render_visible_tiles(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset) {
    Rectangle cam_bounds = { camera.target.x / camera.zoom,
        camera.target.y / camera.zoom,
        GetScreenWidth() / camera.zoom,
        GetScreenHeight() / camera.zoom };

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int tileid = get_tile(x, y);
            if (tileid == -1) {
                continue;
            }
            Tile tile = tileset.get_tile(tileid);
            Vector2 position = tile_to_world(x, y);
            Rectangle tileBounds = { position.x, position.y, TILE_SIZE * ZOOM_MUTLIPLE, TILE_SIZE * ZOOM_MUTLIPLE };
            if (CheckCollisionRecs(cam_bounds, tileBounds)) {
                Rectangle sourceRect = tile.sprite.texture_rect;
                Rectangle destRect = { position.x, position.y, sourceRect.width * ZOOM_MUTLIPLE, sourceRect.height * ZOOM_MUTLIPLE };
                DrawTexturePro(spritesheet, sourceRect, destRect, tile.sprite.origin, 0.0f, WHITE);
            }
        }
    }
}

TileMap::~TileMap() {
    unload_resources();
}

void TileMap::unload_resources() {
    TileMap_tiles.clear();
    loaded_chunks.clear();
    loaded_chunk_coords.clear();
}

TileSet::~TileSet() {
    tiles.clear();
}

//Render the 32by32 map
void TileMap::render_small_tilemap(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset) {
    Vector2 vOrigin = { camera.offset.x - camera.target.x, camera.offset.y - camera.target.y };

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int tileid = get_tile(x, y);
            if (tileid == -1) {
                continue;
            }
            Tile tile = tileset.get_tile(tileid);
            Vector2 position = tile_to_world(x, y);
            position.x += camera.offset.x - camera.target.x;
            position.y += camera.offset.y - camera.target.y;

            Rectangle sourceRect = tile.sprite.texture_rect;
            Rectangle destRect = { position.x, position.y,
    (float)(sourceRect.width * ZOOM_MUTLIPLE), (float)(sourceRect.height * ZOOM_MUTLIPLE) };
            DrawTexturePro(spritesheet, sourceRect, destRect, vOrigin, 0.0f, WHITE);
        }
    }
}

void TileMap::render_culling_chunks(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset, bool bculling) {
    int load_radius = LOAD_RADIUS_NUM;
    float chunk_size_in_world_units = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / camera.zoom;

    Vector2 center_chunk = {
        std::round(camera.target.x / camera.zoom / chunk_size_in_world_units),
        std::round(camera.target.y / camera.zoom / chunk_size_in_world_units)
    };

    Rectangle cam_bounds = bculling // Indicates whether additional border clipping is enabled
        ? Rectangle{
            camera.target.x - (GetScreenWidth() - (float)CULLING_EDGE) / (2.0f * camera.zoom),
            camera.target.y - (GetScreenHeight() - (float)CULLING_EDGE) / (2.0f * camera.zoom),
            (GetScreenWidth() - (float)CULLING_EDGE) / camera.zoom,
            (GetScreenHeight() - (float)CULLING_EDGE) / camera.zoom
    }
        : Rectangle{
            camera.target.x - GetScreenWidth() / (2.0f * camera.zoom),
            camera.target.y - GetScreenHeight() / (2.0f * camera.zoom),
            GetScreenWidth() / camera.zoom,
            GetScreenHeight() / camera.zoom
    };

    for (int chunk_x = static_cast<int>(center_chunk.x) - load_radius; chunk_x <= static_cast<int>(center_chunk.x) + load_radius; ++chunk_x) {
        for (int chunk_y = static_cast<int>(center_chunk.y) - load_radius; chunk_y <= static_cast<int>(center_chunk.y) + load_radius; ++chunk_y) {
            if (chunk_x < 0 || chunk_y < 0 || chunk_x * CHUNK_SIZE >= width || chunk_y * CHUNK_SIZE >= height) {
                continue;
            }

            if (!is_chunk_loaded(chunk_x, chunk_y)) {
                load_chunk(chunk_x, chunk_y, tileset);
            }

            for (const auto& chunk : loaded_chunks) {
                if (chunk.chunk_x == chunk_x && chunk.chunk_y == chunk_y) {
                    for (int x = 0; x < CHUNK_SIZE; ++x) {
                        for (int y = 0; y < CHUNK_SIZE; ++y) {
                            Vector2 position = tile_to_world(chunk.chunk_x * CHUNK_SIZE + x, chunk.chunk_y * CHUNK_SIZE + y);
                            Rectangle tileBounds = {
                                position.x, position.y,
                                TILE_SIZE * ZOOM_MUTLIPLE, TILE_SIZE * ZOOM_MUTLIPLE
                            };
                            //Get the full tile through tileset and tile ID
                            if (CheckCollisionRecs(cam_bounds, tileBounds)) {
                                Tile tile = tileset.get_tile(chunk.tilesIDs[y * CHUNK_SIZE + x]);
                                Rectangle sourceRect = tile.sprite.texture_rect;
                                Rectangle destRect = {
                                    position.x, position.y,
                                    (float)(sourceRect.width * ZOOM_MUTLIPLE),
                                    (float)(sourceRect.height * ZOOM_MUTLIPLE)
                                };
                                DrawTexturePro(spritesheet, sourceRect, destRect, tile.sprite.origin, 0.0f, WHITE);
                            }
                        }
                    }
                }
            }
        }
    }
}

void TileMap::render_visible_chunks(Texture2D& spritesheet, Camera2D& camera, const TileSet& tileset)
{
    int load_radius = LOAD_RADIUS_NUM;
    float chunk_size_in_world_units = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / camera.zoom;// 2 //mod
    Vector2 center_chunk;//From center chunk to the other four direction chunks
    center_chunk.x = std::round(camera.target.x / camera.zoom / chunk_size_in_world_units);
    center_chunk.y = std::round(camera.target.y / camera.zoom / chunk_size_in_world_units);

    for (int chunk_x = static_cast<int>(center_chunk.x) - load_radius; chunk_x <= static_cast<int>(center_chunk.x) + load_radius; ++chunk_x) {
        for (int chunk_y = static_cast<int>(center_chunk.y) - load_radius; chunk_y <= static_cast<int>(center_chunk.y) + load_radius; ++chunk_y) {
            if (chunk_x < 0 || chunk_y < 0 || chunk_x * CHUNK_SIZE >= width || chunk_y * CHUNK_SIZE >= height) {
                continue; // Used the check the boundary and ensure whether to load the map or not
            }
            if (!is_chunk_loaded(chunk_x, chunk_y)) {
                load_chunk(chunk_x, chunk_y, tileset);
            }
            for (const auto& chunk : loaded_chunks) {
                if (chunk.chunk_x == chunk_x && chunk.chunk_y == chunk_y) {
                    for (int x = 0; x < CHUNK_SIZE; ++x) {
                        for (int y = 0; y < CHUNK_SIZE; ++y)
                        {
                            Tile tile = tileset.get_tile(chunk.tilesIDs[y * CHUNK_SIZE + x]);
                            Vector2 position = tile_to_world(chunk.chunk_x * CHUNK_SIZE + x, chunk.chunk_y * CHUNK_SIZE + y);
                            Rectangle sourceRect = tile.sprite.texture_rect;
                            Rectangle destRect = { position.x, position.y,(float)(sourceRect.width * ZOOM_MUTLIPLE),
                                (float)(sourceRect.height * ZOOM_MUTLIPLE) }; // Adjusted for 2x scale 
                            DrawTexturePro(spritesheet, sourceRect, destRect, tile.sprite.origin, 0.0f, WHITE);
                        }
                    }
                }
            }
        }
    }
}
