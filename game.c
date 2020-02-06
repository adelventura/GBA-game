#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gba.h"
#include "game.h"
#include "render.h"

#include "images/shark.h"
#include "images/fish.h"
#include "images/bg.h"
#include "images/splash_screen.h"
#include "images/win.h"
#include "images/lose.h"

// clamps a value (current) between min and max
// returns the clamped value
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

// checks for a collision between a player and fish
// returns true if a collision occurred
bool check_collision(Shark *player, Fish *fish)
{
    return player->x < fish->x + fish->size &&
           player->x + player->size > fish->x &&
           player->y < fish->y + fish->size &&
           player->y + player->size > fish->y;
}

// initializes a new fish with a random state for immediate use within the game
// fish placed at edges of screen with direction inwards
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

// initializes new game with player in the center of screen
// creates a new set of fish
// starts at the splash_screen
State new_game(void)
{
    State state = {
        .state = START,
        .player = {
            .x = WIDTH / 2,
            .y = HEIGHT / 2,
            .size = INITIAL_SIZE},
        .lives = 3};

    for (int i = 0; i < NUM_FISH; i++)
    {
        state.fish[i] = new_fish();
    }

    return state;
}

// watches for select key press
// resets game state and sends to splash_screen
//
// state: the current state
// buttons: current button state
void handle_select(State *state, u32 buttons)
{
    if (KEY_DOWN(BUTTON_SELECT, buttons))
    {
        *state = new_game();
    }
}

// checks for any key press on
// updates game state to PLAY
//
// state: the current state
// prev: previous button state
// curr: current button state
void start_on_keypress(State *state, u32 prev, u32 curr)
{
    UNUSED(prev);
    // if no buttons pressed in previous frame, and a button pressed currently
    if ((prev & BUTTONS) == BUTTONS && curr != BUTTONS)
    {
        *state = new_game();
        state->state = PLAY;

        drawFullScreenImageDMA(bg);
    }
}

// update game state
// checking for collisions, generating fish, implementing physics, handling key presses
//
// state: current state
// buttons: current button state
void update(State *state, u32 buttons)
{
    Shark *player = &state->player;

    //
    // update fish
    //

    for (int i = 0; i < NUM_FISH; i++)
    {
        Fish *fish = &state->fish[i];
        fish->x += fish->vx;
        fish->y += fish->vy;

        // check for collisions
        if (check_collision(player, fish))
        {
            // if fish smaller than player, eat fish, create a new fish
            if (fish->size <= player->size)
            {
                player->size += (fish->size / 2);
                player->size = clamp(1, WINNING_SIZE, player->size);

                if (state->lives < 3)
                {
                    state->lives += 1;
                }
            }
            // if fish larger than player, decrement lives
            else
            {
                state->lives -= 1;
            }

            state->fish[i] = new_fish();
        }
        // if a fish moves offscreen, replace it with a new fish
        else if ((fish->x < 0 && fish->vx < 0) || (fish->x > (fish->size + WIDTH) && fish->vx > 0) || (fish->y < 0 && fish->vy < 0) || (fish->y > (fish->size + HEIGHT) && fish->vy > 0))
        {
            state->fish[i] = new_fish();
        }
    }

    //
    // Handle Key Presses
    //

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

    //
    // Update Player Position
    //

    player->x = clamp(0, WIDTH - player->size, player->x);
    player->y = clamp(0, HEIGHT - player->size, player->y);

    //
    // Check for win/lose conditions
    //

    if (player->size >= WINNING_SIZE)
    {
        state->state = WIN;
    }

    if (state->lives == 0)
    {
        state->state = LOSE;
    }
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

        // handle state
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
