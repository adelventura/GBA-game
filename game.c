#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "game.h"

#include "images/shark.h"
#include "images/fish.h"
#include "images/bg.h"
#include "images/splash_screen.h"
#include "images/win.h"
#include "images/lose.h"

const int SPEED = 3;
const int INITIAL_SIZE = 10;
const int WINNING_SIZE = 100;

typedef struct state
{
    GBAState state;

    Shark player;

    Fish fish[10];
    int number_of_fish;
} State;

void render_score(Shark *shark)
{
    char *formatted = (char *)malloc(50 * sizeof(char));
    sprintf(formatted, "score: %d", shark->size - INITIAL_SIZE);
    drawString(2, HEIGHT - 10, formatted, YELLOW);
    free(formatted);
}

void render_shark(Shark *shark)
{
    drawRectDMA(shark->x, shark->y, shark->size, shark->size, RED);
}

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

State new_game()
{
    return (State){
        .state = START,
        .player = {
            .x = 10,
            .y = 10,
            .size = INITIAL_SIZE}};
}

void handle_select(State *state, u32 buttons)
{
    if (KEY_DOWN(BUTTON_SELECT, buttons))
    {
        *state = new_game();
    }
}

void start_on_keypress(State *state, u32 buttons)
{
    if (buttons != BUTTONS)
    {
        *state = new_game();
        state->state = PLAY;
    }
}

void play(State *state, u32 buttons)
{
    drawFullScreenImageDMA(bg);

    Shark *player = &state->player;
    render_shark(player);
    render_score(player);

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
            start_on_keypress(&state, currentButtons);
            break;
        case PLAY:
            play(&state, currentButtons);
            break;
        case WIN:
            drawFullScreenImageDMA(win);
            start_on_keypress(&state, currentButtons);
            break;
        case LOSE:
            drawFullScreenImageDMA(lose);
            start_on_keypress(&state, currentButtons);
            break;
        }

        previousButtons = currentButtons; // Store the current state of the buttons
    }

    UNUSED(previousButtons); // You can remove this once previousButtons is used
    return 0;
}
