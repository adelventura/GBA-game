#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "game.h"

#include "images/shark.h"
#include "images/fish.h"
#include "images/start_screen.h"

int main(void)
{
    // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial game state
    GBAState state = START;

    char *formatted = (char *)malloc(50 * sizeof(char));

    int x = 10;
    while (1)
    {
        currentButtons = BUTTONS; // Load the current state of the buttons

    waitForVBlank();
    drawFullScreenImageDMA(start_screen);
    drawRectDMA(x, 10, 10, 10, RED);

    sprintf(formatted, "x: %d", x);
    drawString(2, HEIGHT - 10, formatted, YELLOW);

    x += 1;

        switch (state)
        {
        case START: break;
        case PLAY: break;
        case WIN: break;
        case LOSE: break;
        }

        previousButtons = currentButtons; // Store the current state of the buttons
    }

    free(formatted);

    UNUSED(previousButtons); // You can remove this once previousButtons is used
    return 0;
}
