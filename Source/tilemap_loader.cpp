#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> 
#include "raylib.h"
#include "tile.h"         
#include "sprite.h"
#include "tilemap_loader.h"

using json = nlohmann::json;

TileMap loadTileMapFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open tilemap JSON file: " << filename << std::endl;
        return TileMap();
    }

    json jsonData;
    file >> jsonData;

    // Load tileset and map settings
    std::string tilesetPath = jsonData["tileset"];
    int tileSize = jsonData["tileSize"];
    int mapWidth = jsonData["mapSize"]["width"];
    int mapHeight = jsonData["mapSize"]["height"];

    Texture2D tilesetTexture = LoadTexture(tilesetPath.c_str());
    TileSet tileset;
    int tileCountX = tilesetTexture.width / tileSize;
    int tileCountY = tilesetTexture.height / tileSize;

    for (int y = 0; y < tileCountY; ++y) {
        for (int x = 0; x < tileCountX; ++x) {
            Tile tile = {};
            tile.sprite = get_sprite_from_sheet(x, y, tileSize, tileSize);
            tile.solid = false; 
            tileset.add_tile(tile);
        }
    }
    TileMap tileMap(mapWidth, mapHeight);

    // Load layer data
    if (!jsonData.contains("layers") || jsonData["layers"].empty() || !jsonData["layers"][0].contains("data")) {
        std::cerr << "Error: Missing tile layer data in JSON file." << std::endl;
        return tileMap; // Return empty TileMap
    }
    std::vector<int> layerData = jsonData["layers"][0]["data"];

    // Ensure layer data matches map dimensions
    size_t expectedSize = static_cast<size_t>(mapWidth * mapHeight);
    if (layerData.size() < expectedSize) {
        std::cerr << "Warning: Layer data size mismatch. Filling missing tiles with default value 0." << std::endl;
        layerData.resize(expectedSize, 0); // Automatically fill the lost data
    }
    else if (layerData.size() > expectedSize) {
        std::cerr << "Warning: Layer data size exceeds map dimensions. Truncating extra tiles." << std::endl;
        layerData.resize(expectedSize); 
    }


    // Set tile data in TileMap
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int index = y * mapWidth + x;
            int tileID = layerData[index];
            if (tileID >= 0 && tileID < static_cast<int>(tileset.get_tile_count())) {
                tileMap.set_tile(x, y, tileID); // Using Tile ID set the tile
            }
        }
    }
    return tileMap;
}


