#ifndef GAME_H
#define GAME_H

#include "gba.h"

/* TODO: */

// Create any necessary structs //

/*
* For example, for a Snake game, one could be:
*
* typedef struct snake {
*   int heading;
*   int length;
*   int x;
*   int y;
* } Snake;
*
*
* Example of a struct to hold state machine data:
*
* typedef struct state {
*   int currentState;
*   int nextState;
* } State
*
*/

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

typedef struct state
{
    int currentState;
    int nextState;
} State;

#endif
