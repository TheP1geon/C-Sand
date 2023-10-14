#include "include/object.h"
#include "include/color.h"

Color colorFromType(Object obj) {
    switch (obj.type) {
        case SAND:
            return YELLOW;
        case STONE:
            return LIGHTGRAY;
        case WATER:
            return BLUE;
        default:
            return BLACK;
    }
}
