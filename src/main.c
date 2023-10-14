#include <SDL2/SDL_keycode.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#include "include/types.h"
#include "include/color.h"
#include "include/rect.h"
#include "include/object.h"

// Constants
#define SCREEN_WIDTH     700
#define SCREEN_HEIGHT    700

#define CELL_SIZE        5
#define GRID_WIDTH       SCREEN_WIDTH / CELL_SIZE
#define GRID_HEIGHT      SCREEN_HEIGHT / CELL_SIZE

#define FPS              30
#define BACKGROUND_COLOR BLACK
// __Constants

// Macros
#define GRID_INDEX(x, y)    (x) + (y) * GRID_WIDTH 
#define GRID_AT(x, y)       grid[GRID_INDEX(x, y)]
#define CLEAR_GRID_AT(x, y) GRID_AT(x, y) = (Object){};
// __Macros

Object grid[GRID_HEIGHT * GRID_WIDTH] = {};
ObjectType current_material = SAND;

// Variables to track mouse state and previous position
bool mouseDown = false;
i32 prevMouseX = -1;
i32 prevMouseY = -1;

void init_sim(void) {
    srand(time(NULL));
}

void step_sim(void) {
    for (i32 y = GRID_HEIGHT - 1; y >= 0; y--) {
        for (i32 x = GRID_WIDTH - 1; x >= 0; x--) {
            Object* obj = &grid[GRID_INDEX(x, y)];
            
            switch (obj->type) {
                case None:
                    break;
                case SAND: {
                    if (y == GRID_HEIGHT - 1) {
                        break;
                    }

                    Object* obj_down = &GRID_AT(x, y + 1); 

                    Object* obj_left = &GRID_AT(x - 1, y + 1); 
                    Object* obj_down_left = &GRID_AT(x - 1, y); 

                    Object* obj_right = &GRID_AT(x + 1, y + 1); 
                    Object* obj_down_right = &GRID_AT(x + 1, y); 

                    bool left_okay =  x != 0 && 
                        obj_down_left->type != STONE && obj_down_left->type != SAND &&
                        obj_left->type != STONE && obj_left->type != SAND;
                    bool right_okay = x != GRID_WIDTH-1 && 
                        obj_down_right->type != STONE && obj_down_right->type != SAND &&
                        obj_right->type != STONE && obj_right->type != SAND;

                    if (obj_down->type == None) {
                        *obj_down = *obj;
                        CLEAR_GRID_AT(x, y);
                    }

                    if (obj_down->type == STONE || obj_down->type == SAND) { 

                        if (left_okay && right_okay) {
                            if (rand()%2) {*obj_down_left = *obj;} 
                            else {*obj_down_right = *obj;} 

                            CLEAR_GRID_AT(x, y);
                        } else if (left_okay) {
                            *obj_down_left = *obj; 
                            CLEAR_GRID_AT(x, y);
                        } else if (right_okay) {
                            *obj_down_right = *obj; 
                            CLEAR_GRID_AT(x, y);
                        }
                    }

                    break;
                }
                case STONE:
                    break;
                default:
                    break;
            }
        }
    }
}

void handleMouseDrag(i32 x, i32 y) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        if (GRID_AT(x, y).type != None) {
            return;
        }
        GRID_AT(x, y) = (Object){current_material, 0};
    }
}

void display(SDL_Renderer* renderer) {
    for (i32 y = GRID_HEIGHT - 1; y >= 0; y--) {
        for (i32 x = GRID_WIDTH - 1; x >= 0; x--) {
            Color squareColor = colorFromType(grid[GRID_INDEX(x, y)]);
            Rect r = {{x * CELL_SIZE, y * CELL_SIZE}, CELL_SIZE, CELL_SIZE, squareColor};
            drawFillRect(renderer, r);
        }
    }
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    bool paused = true;
    SDL_Event e;
    u32 frameStart, frameTime;
    const u32 frameDelay = 1000/FPS;

    init_sim();
    
    while (!quit) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        mouseDown = true;
                        prevMouseX = e.button.x / CELL_SIZE;
                        prevMouseY = e.button.y / CELL_SIZE;
                        handleMouseDrag(prevMouseX, prevMouseY);
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    if (mouseDown) {
                        i32 x = e.motion.x / CELL_SIZE;
                        i32 y = e.motion.y / CELL_SIZE;
                        if (x != prevMouseX || y != prevMouseY) {
                            handleMouseDrag(x, y);
                            prevMouseX = x;
                            prevMouseY = y;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        mouseDown = false;
                    }
                    break;
                }
                case SDL_KEYDOWN: {
                    SDL_KeyCode key = e.key.keysym.sym;
                    
                    switch (key) {
                        case SDLK_a:
                            current_material = SAND;
                            break;
                        case SDLK_s:
                            current_material = STONE;
                            break;
                        case SDLK_RIGHT:
                            step_sim();
                            break;
                        case SDLK_SPACE:
                            paused = !paused;
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, ColorParam(BACKGROUND_COLOR));
        SDL_RenderClear(renderer);

        if (!paused) {
            step_sim();
        }

        // Start Drawing
        display(renderer);

        // End Drawing
        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
