#ifndef GAME_H
#define GAME_H

#include "gba.h"

typedef enum
{
    START,
    PLAY,
    WIN,
    LOSE,
} GBAState;

typedef struct fish
{
    int direction;
    int size;
    int x;
    int y;
} Fish;

typedef struct shark
{
    int size;
    int x;
    int y;
} Shark;

#endif
