#include "render.h"

#include <stdio.h>
#include <stdlib.h>

#include "images/bg.h"
#include "images/shark.h"

char score_buffer[15];

// render the score to bottom left of screen
void render_score(Shark *shark)
{
    sprintf(score_buffer, "score: %d", shark->size - INITIAL_SIZE);
    drawString(2, HEIGHT - 10, score_buffer, YELLOW);
}

// draws player at current size to current location
void render_player(Shark *player)
{
    drawRectDMA(player->x, player->y, player->size, player->size, GRAY);
}

// draws fish at size to current location
void render_fish(Fish *fish)
{
    drawRectDMA(fish->x, fish->y, fish->size, fish->size, BLUE);
}

// draw current number of lives to top left of screen
void render_lives(State *state)
{
    for (int i = 0; i < state->lives; i++)
    {
        drawImageDMA(2 + (i * 12), 5, 10, 10, shark);
    }
}

// render the current game state (fish, player, score, lives)
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

// clear the previous game state. This is used to only redraw as small of the
// screen as possible on each frame to have more time to draw the next frame.
void clear(State *state)
{
    if (state->state == WIN)
    {
        // ignore if we have won
        return;
    }

    // clear the score and lives
    drawFullScreenImagePatchDMA(2, HEIGHT - 10, 60, 10, bg);
    drawFullScreenImagePatchDMA(2, 2, 40, 12, bg);

    // clear the previous fish positions
    for (int i = 0; i < NUM_FISH; i++)
    {
        Fish *fish = &state->fish[i];
        drawFullScreenImagePatchDMA(fish->x, fish->y, fish->size, fish->size, bg);
    }

    // clear the previous player position
    Shark *shark = &state->player;
    drawFullScreenImagePatchDMA(shark->x, shark->y, shark->size, shark->size, bg);
}