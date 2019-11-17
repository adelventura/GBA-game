#include "render.h"

#include <stdio.h>
#include <stdlib.h>

#include "images/bg.h"

char score_buffer[15];

void render_score(Shark *shark)
{
    sprintf(score_buffer, "score: %d", shark->size - INITIAL_SIZE);
    drawString(2, HEIGHT - 10, score_buffer, YELLOW);
}

void render_shark(Shark *shark)
{
    drawRectDMA(shark->x, shark->y, shark->size, shark->size, RED);
}

void render_fish(Fish *fish)
{
    drawRectDMA(fish->x, fish->y, fish->size, fish->size, YELLOW);
}

void render(State *state)
{
    render_score(&state->player);

    for (int i = 0; i < NUM_FISH; i++)
    {
        render_fish(&state->fish[i]);
    }

    render_shark(&state->player);
}

void clear(State *state)
{
    // clear the score
    drawFullScreenImagePatchDMA(2, HEIGHT - 10, 30, 10, bg);

    for (int i = 0; i < NUM_FISH; i++)
    {
        Fish *fish = &state->fish[i];
        drawFullScreenImagePatchDMA(fish->x, fish->y, fish->size, fish->size, bg);
    }

    Shark *shark = &state->player;
    drawFullScreenImagePatchDMA(shark->x, shark->y, shark->size, shark->size, bg);
}