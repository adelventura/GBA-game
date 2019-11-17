#include "render.h"

#include <stdio.h>
#include <stdlib.h>

#include "images/bg.h"
#include "images/shark.h"

char score_buffer[15];

void render_score(Shark *shark)
{
    sprintf(score_buffer, "score: %d", shark->size - INITIAL_SIZE);
    drawString(2, HEIGHT - 10, score_buffer, YELLOW);
}

void render_player(Shark *player)
{
    drawRectDMA(player->x, player->y, player->size, player->size, GRAY);
}

void render_fish(Fish *fish)
{
    drawRectDMA(fish->x, fish->y, fish->size, fish->size, YELLOW);
}

void render_lives(State *state)
{
    for (int i = 0; i < state->lives; i++)
    {
        drawImageDMA(2 + (i * 12), 2, 10, 10, shark);
    }
}

void render(State *state)
{
    for (int i = 0; i < NUM_FISH; i++)
    {
        render_fish(&state->fish[i]);
    }

    render_player(&state->player);

    render_score(&state->player);
    render_lives(state);
}

void clear(State *state)
{
    if (state->state == WIN)
    {
        return;
    }

    // clear the score and lives
    drawFullScreenImagePatchDMA(2, HEIGHT - 10, 50, 10, bg);
    drawFullScreenImagePatchDMA(2, 2, 40, 10, bg);

    for (int i = 0; i < NUM_FISH; i++)
    {
        Fish *fish = &state->fish[i];
        drawFullScreenImagePatchDMA(fish->x, fish->y, fish->size, fish->size, bg);
    }

    Shark *shark = &state->player;
    drawFullScreenImagePatchDMA(shark->x, shark->y, shark->size, shark->size, bg);
}