#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "game.h"
#include "render.h"

#include "images/shark.h"
#include "images/fish.h"
#include "images/bg.h"
#include "images/splash_screen.h"
#include "images/win.h"
#include "images/lose.h"

int clamp(int min, int max, int current)
{
    if (current < min)
    {
        return min;
    }
    if (current > max)
    {
        return max;
    }
    return current;
}

Fish new_fish(void)
{
    int size = randint(SMALL_FISH, BIG_FISH);

    if (randint(0, 100) > 50)
    {
        // top/bottom
        int vx = (randint(0, 100) > 50 ? -1 : 1) * randint(1, 2);
        int vy = (randint(0, 100) > 50 ? -1 : 1) * randint(1, 2);

        return (Fish){
            .size = size,
            .x = randint(0, WIDTH),
            .y = (vy > 0) ? 0 : HEIGHT - size,
            .vx = vx,
            .vy = vy};
    }
    else
    {
        // left/right
        int vx = (randint(0, 100) > 50 ? -1 : 1) * randint(1, 2);
        int vy = (randint(0, 100) > 50 ? -1 : 1) * randint(1, 2);

        return (Fish){
            .size = size,
            .x = (vx > 0) ? 0 : WIDTH - size,
            .y = randint(0, HEIGHT),
            .vx = vx,
            .vy = vy};
    }
}

State new_game(void)
{
    State state = {
        .state = START,
        .player = {
            .x = 10,
            .y = 10,
            .size = INITIAL_SIZE}};

    for (int i = 0; i < NUM_FISH; i++)
    {
        state.fish[i] = new_fish();
    }

    return state;
}

void handle_select(State *state, u32 buttons)
{
    if (KEY_DOWN(BUTTON_SELECT, buttons))
    {
        *state = new_game();
    }
}

void start_on_keypress(State *state, u32 prev, u32 curr)
{
    UNUSED(prev);
    if (curr != BUTTONS)
    {
        *state = new_game();
        state->state = PLAY;

        drawFullScreenImageDMA(bg);
    }
}

void update(State *state, u32 buttons)
{
    Shark *player = &state->player;

    for (int i = 0; i < NUM_FISH; i++)
    {
        Fish *fish = &state->fish[i];
        fish->x += fish->vx;
        fish->y += fish->vy;

        if ((fish->x < 0 && fish->vx < 0) || (fish->x > (fish->size + WIDTH) && fish->vx > 0) || (fish->y < 0 && fish->vy < 0) || (fish->y > (fish->size + HEIGHT) && fish->vy > 0))
        {
            state->fish[i] = new_fish();
        }
    }

    if (KEY_DOWN(BUTTON_DOWN, buttons))
    {
        player->y += SPEED;
    }

    if (KEY_DOWN(BUTTON_UP, buttons))
    {
        player->y -= SPEED;
    }

    if (KEY_DOWN(BUTTON_LEFT, buttons))
    {
        player->x -= SPEED;
    }

    if (KEY_DOWN(BUTTON_RIGHT, buttons))
    {
        player->x += SPEED;
    }

    player->x = clamp(0, WIDTH - player->size, player->x);
    player->y = clamp(0, HEIGHT - player->size, player->y);
}

int main(void)
{
    // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial game state
    State state = new_game();

    while (1)
    {
        currentButtons = BUTTONS; // Load the current state of the buttons
        waitForVBlank();

        // reset on select
        handle_select(&state, currentButtons);

        switch (state.state)
        {
        case START:
            drawFullScreenImageDMA(splash_screen);
            start_on_keypress(&state, previousButtons, currentButtons);
            break;
        case PLAY:
            clear(&state);
            update(&state, currentButtons);
            render(&state);
            break;
        case WIN:
            drawFullScreenImageDMA(win);
            start_on_keypress(&state, previousButtons, currentButtons);
            break;
        case LOSE:
            drawFullScreenImageDMA(lose);
            start_on_keypress(&state, previousButtons, currentButtons);
            break;
        }

        previousButtons = currentButtons; // Store the current state of the buttons
    }

    return 0;
}
