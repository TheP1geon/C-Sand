#include <stdlib.h>
#include "include/object.h"
#include "include/color.h"

Color colorFromType(Object obj) {
    switch (obj.type) {
        case SAND: {
            i32 rand_num = rand() % 3;
            switch (rand_num) {
                case 0: return (Color){255, 255, 0, 255}; 
                case 1: return (Color){178, 201, 6, 255};
                case 2: return (Color){233, 252, 90, 255};
                default: return YELLOW;
            }
        }
            break;
        case STONE: {
            i32 rand_num = rand() % 3;
            switch (rand_num) {
                case 0: return (Color){100, 100, 100, 255};
                case 1: return (Color){150, 150, 150, 255}; 
                case 2: return (Color){200, 200, 200, 255};
                default: return GRAY;
            }
        }
            break;
        case WATER:
            return WATER_COLOR;
        default:
            return BLACK;
    }
}
