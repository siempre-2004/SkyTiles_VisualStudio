#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"

struct Sprite {
    Rectangle texture_rect;
    Rectangle sourceRect;
    Vector2 origin;

    Sprite() : texture_rect{ 0, 0, 0, 0 }, sourceRect{ 0, 0, 0, 0 }, origin{ 0, 0 } {}
};

Sprite get_sprite_from_sheet(int x, int y, int sprite_w, int sprite_h);
void DrawSprite(Texture spritesheet, Sprite sprite, Vector2 position, Color tint);

#endif  