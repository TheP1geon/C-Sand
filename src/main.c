#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
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

#define CELL_SIZE        2
#define GRID_WIDTH       (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT      (SCREEN_HEIGHT / CELL_SIZE)

#define FPS              240
#define BACKGROUND_COLOR BLACK
// __Constants

// Macros
#define GRID_INDEX(x, y)    (x) + (y) * GRID_WIDTH 
#define GRID_AT(x, y)       grid[GRID_INDEX(x, y)]
#define CLEAR_GRID_AT(x, y) GRID_AT(x, y) = (Object){};
// __Macros

Object grid[GRID_HEIGHT * GRID_WIDTH] = {};
i32 indicies[GRID_HEIGHT * GRID_WIDTH] = {};
ObjectType current_material = SAND;

// Variables to track mouse state and previous position
bool mouseDown = false;
i32 prevMouseX = -1;
i32 prevMouseY = -1;

i32 BRUSH_RADIUS = 15;

void shuffle_indicies(void) {
    for (i32 i = GRID_HEIGHT * GRID_WIDTH - 1; i > 0; i--) {
        i32 j = rand() % (i + 1);
        i32 temp = indicies[i];
        indicies[i] = indicies[j];
        indicies[j] = temp;
    }
}

void init_sim(void) {
    srand(time(NULL));

    for (i32 i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
        indicies[i] = i;
    }

    shuffle_indicies();
}

void swap(i32 x1, i32 y1, i32 x2, i32 y2) {
    Object* obj1 = &GRID_AT(x1, y1);
    Object* obj2 = &GRID_AT(x2, y2);
    
    Object temp = *obj1;
    *obj1 = *obj2;
    *obj2 = temp;
}

void step_sim(void) {
    shuffle_indicies();
    for (i32 i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
        i32 index = indicies[i];
        i32 x = index % GRID_WIDTH; 
        i32 y = index / GRID_WIDTH;

        Object* obj = &GRID_AT(x, y);
        
        switch (obj->type) {
            case None:
                break;
            case SAND: {
                if (y == GRID_HEIGHT - 1) {
                    continue;
                }

                Object* obj_down = &GRID_AT(x, y + 1); 

                Object* obj_left = &GRID_AT(x - 1, y + 1); 
                Object* obj_down_left = &GRID_AT(x - 1, y); 

                Object* obj_right = &GRID_AT(x + 1, y + 1); 
                Object* obj_down_right = &GRID_AT(x + 1, y); 

                bool left_okay =  x != 0 && 
                    (obj_down_left->type == None || obj_down_left->type == WATER) &&
                    (obj_left->type == None || obj_left->type == WATER);

                bool right_okay =  x != GRID_WIDTH-1 && 
                    (obj_down_right->type == None || obj_down_right->type == WATER) &&
                    (obj_right->type == None || obj_right->type == WATER);

                if (obj_down->type == None || obj_down->type == WATER) {
                    swap(x, y, x, y+1);
                }

                if (left_okay && right_okay) {
                    if (rand()%2) {swap(x, y, x-1, y+1);} 
                    else {swap(x, y, x+1, y+1);} 

                } else if (left_okay) {
                    swap(x, y, x - 1, y + 1);
                } else if (right_okay) { 
                    swap(x, y, x + 1, y + 1);
                }
               

                break;
            }
            case STONE:
                break;
            case WATER: {
                if (y == GRID_HEIGHT - 1) {
                    // Water is at the bottom, no need to check further
                    continue;
                }

                // Check if there's empty space beneath
                if (GRID_AT(x, y + 1).type == None) {
                    swap(x, y, x, y + 1);
                } else {
                    // No empty space beneath, check left and right
                    bool left_okay = (x != 0) && GRID_AT(x - 1, y).type == None;
                    bool right_okay = (x != GRID_WIDTH - 1) && GRID_AT(x + 1, y).type == None;

                    if (left_okay && right_okay) {
                        if (rand()%2) {
                            swap(x, y, x - 1, y);
                        } else {
                            swap(x, y, x + 1, y);
                        }
                    } else if (left_okay) {
                        swap(x, y, x - 1, y);
                    } else if (right_okay) {
                        swap(x, y, x + 1, y);
                    }
                }
            }
                break;
            default:
                break;
        }
    }
}


void handleMouseDrag(i32 x, i32 y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return;
    }

    for (i32 c_y = -BRUSH_RADIUS; c_y <= BRUSH_RADIUS; ++c_y) {
        for (i32 c_x = -BRUSH_RADIUS; c_x <= BRUSH_RADIUS; ++c_x) {
            if (c_y * c_y + c_x * c_x > BRUSH_RADIUS * BRUSH_RADIUS) {
                continue;;
            }

            if (x + c_x < 0 || x + c_x >= GRID_WIDTH || y + c_y < 0 || y + c_y >= GRID_HEIGHT) {
                continue;
            }

            if (rand() % 5 == 0) {
                continue;
            }

            Object* obj = &GRID_AT(x+c_x, y+c_y); 
            if (obj->type == None) {
                obj->type = current_material;
                obj->color = colorFromType(*obj);
            } else if (current_material == None && obj->type != None) {
                obj->type = None;
                obj->color = BACKGROUND_COLOR;
            }
        }
    }
}

void display(SDL_Renderer* renderer) {
    for (i32 i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
        i32 index = indicies[i];
        i32 x = index % GRID_WIDTH; 
        i32 y = index / GRID_WIDTH;
        Color squareColor = GRID_AT(x, y).color;
        Rect r = {{x * CELL_SIZE, y * CELL_SIZE}, CELL_SIZE, CELL_SIZE, squareColor};
        drawFillRect(renderer, r);
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
    bool paused = false;
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
                    SDL_SetRenderDrawColor(renderer, ColorParam(DARKGRAY));
                    for (i32 y = -BRUSH_RADIUS; y <= BRUSH_RADIUS; ++y) {
                        for (i32 x = -BRUSH_RADIUS; x <= BRUSH_RADIUS; ++x) {
                            int distance_squared = x * x + y * y;
                            if (distance_squared >= (BRUSH_RADIUS - 1) * (BRUSH_RADIUS - 1) && distance_squared <= BRUSH_RADIUS * BRUSH_RADIUS) {
                                SDL_RenderDrawPoint(renderer, e.motion.x + x, e.motion.y + y);
                            }
                        }
                    }
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
                        case SDLK_w:
                            current_material = WATER;
                            break;
                        case SDLK_x:
                            current_material = None;
                            break;
                        case SDLK_RIGHT:
                            step_sim();
                            break;
                        case SDLK_UP:
                            if (BRUSH_RADIUS + 1 >= 30) { BRUSH_RADIUS = 29;}
                            BRUSH_RADIUS++;
                            break;
                        case SDLK_DOWN:
                            if (BRUSH_RADIUS - 1 <= 0) { BRUSH_RADIUS = 1; }
                            BRUSH_RADIUS--;
                            break;
                        case SDLK_BACKSPACE: {
                            for (i32 i = 0; i < GRID_HEIGHT; ++i) {
                                for (i32 j = 0; j < GRID_WIDTH; ++j) {
                                    CLEAR_GRID_AT(j, i);
                                }
                            }
                        }
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

        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, ColorParam(BACKGROUND_COLOR));
        SDL_RenderClear(renderer);


        for (i32 i = 0; i < 2; ++i) {
            if (!paused) {
                step_sim();
            }
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
