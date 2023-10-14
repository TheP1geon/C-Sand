#ifndef __OBJECT_H
#define __OBJECT_H

#include "types.h"
#include "color.h"

typedef enum {
    None = 0,
    SAND,
    STONE,
} ObjectType;

typedef struct Object {
    ObjectType type;
    bool has_moved;
} Object;

// Constructors??
#define Sand()  (Object){SAND, 0};
#define Stone() (Object){STONE, 0};

Color colorFromType(Object obj);

#endif  //__OBJECT_H
