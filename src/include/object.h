#ifndef __OBJECT_H
#define __OBJECT_H

#include "types.h"
#include "color.h"

typedef enum {
    None = 0,
    SAND,
    STONE,
    WATER,
} ObjectType;

typedef struct Object {
    ObjectType type;
    bool has_moved;
    Color color;
} Object;

Color colorFromType(Object obj);

#endif  //__OBJECT_H
