#ifndef GAME_H
#define GAME_H

#include "gba.h"

#define SPEED 3
#define INITIAL_SIZE 6
#define WINNING_SIZE 50

#define NUM_FISH 4
#define SMALL_FISH 1
#define BIG_FISH 30

typedef enum
{
    START,
    PLAY,
    WIN,
    LOSE,
} GBAState;

typedef struct fish
{
    int size;
    int x;
    int y;
    int vx;
    int vy;
} Fish;

typedef struct shark
{
    int size;
    int x;
    int y;
} Shark;

typedef struct state
{
    GBAState state;
    Shark player;
    Fish fish[4];
    int lives;
} State;

#endif
