#include "sprite.h"
#include "raylib.h"
#include "particle.h"

Sprite get_sprite_from_sheet(int x, int y, int sprite_w, int sprite_h) {
    Sprite result = {};
    result.texture_rect.x = float(x * sprite_w);
    result.texture_rect.y = float(y * sprite_h);
    result.texture_rect.width = float(sprite_w);
    result.texture_rect.height = float(sprite_h);
    return result;
}

void DrawSprite(Texture spritesheet, Sprite sprite, Vector2 position, Color tint) {
    Rectangle screen_rect = {};//Define the target rectangle to draw on the screen
    screen_rect.x = position.x;
    screen_rect.y = position.y;
    screen_rect.width = sprite.texture_rect.width ; 
    screen_rect.height = sprite.texture_rect.height ;  
    DrawTexturePro(spritesheet, sprite.texture_rect, screen_rect, sprite.origin, 0.0f, tint);
}

