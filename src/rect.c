#include "include/rect.h"
#include "include/color.h"
#include "include/types.h"
#include <SDL2/SDL_render.h>

//  Rectangle Stuff
Rect newRect(vec2 topLeft, i32 width, i32 height, Color color) {
    return (Rect){.topLeft = topLeft, .width = width, .height = height, .color = color}; 
}

void drawFillRect(SDL_Renderer* renderer, Rect rect) {
    SDL_SetRenderDrawColor(renderer, ColorParam(rect.color));
    for (i32 y = rect.topLeft.y; y < rect.topLeft.y + rect.height; ++y) {
        for (i32 x = rect.topLeft.x; x < rect.topLeft.x + rect.width; ++x) {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}
