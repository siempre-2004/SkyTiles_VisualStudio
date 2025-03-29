#pragma once
#ifndef TILEMAP_LOADER_H
#define TILEMAP_LOADER_H

#include <string>
#include "tile.h" 

TileMap loadTileMapFromJSON(const std::string& filename);

#endif
