#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void)
{
    // (1) loop until we're NOT in vBlank anymore:
    // (This prevents counting one VBlank more than once if your app is too fast)
    while (*SCANLINECOUNTER > HEIGHT)
    {
    }

    // (2) keep going until we're in vBlank:
    while (*SCANLINECOUNTER < HEIGHT)
    {
    }
    // (3) Finally, increment the vBlank counter:
    vBlankCounter += 1;
}

static int __qran_seed = 42;
static int qran(void)
{
    __qran_seed = 1664525 * __qran_seed + 1013904223;
    return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max)
{
    return (qran() * (max - min) >> 15) + min;
}

// set screen pixel to color
//
// x: x position of pixel
// y: y position of pixel
// color: color to use
void setPixel(int x, int y, u16 color)
{
    videoBuffer[OFFSET(y, x, WIDTH)] = color;
}

// draw rectangle on screen
//
// x: x position of start of rectangle
// y: y position of start of rectangle
// width: width of rectangle
// height: height of rectangle
// color: color of rectangle
void drawRectDMA(int x, int y, int width, int height, volatile u16 color)
{
    for (int row = y; row < y + height; row += 1)
    {
        // DMA row-by-row
        DMA[3].src = &color;
        DMA[3].dst = &videoBuffer[OFFSET(row, x, WIDTH)];

        DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON | DMA_NOW;
    }
}

// Only draw a portion of a fullscreen image on screen. Used to clear small portions
// of the game screen before drawing the next frame.
//
// x: starting x position of the patch
// y: starting y position of the patch
// width: width of the area to patch
// height: height of the area to patch
// image: pointer to first element of image to use -- requires image to be fullscreen size
void drawFullScreenImagePatchDMA(int x, int y, int width, int height, const u16 *image)
{
    for (int row = 0; row < height; row += 1)
    {
        // DMA row-by-row
        DMA[3].cnt = 0;
        DMA[3].src = &image[OFFSET(y + row, x, WIDTH)];
        DMA[3].dst = &videoBuffer[OFFSET(y + row, x, WIDTH)];

        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_ON;
    }
}

// Draw a fullscreen image to screen
//
// image: pointer to first element of the image to use
void drawFullScreenImageDMA(const u16 *image)
{
    DMA[3].cnt = 0;
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = WIDTH * HEIGHT | DMA_SOURCE_INCREMENT | DMA_ON;
}

//  A function that will draw an arbitrary sized image * onto the screen (with DMA).
//
//  r: row to start drawing the image at
//  c: column to start drawing the image at
//  width: width of the image
//  height: height of the image
//  image: pointer to the first element of the image
void drawImageDMA(int x, int y, int width, int height, const u16 *image)
{
    for (int row = 0; row < height; row += 1)
    {
        DMA[3].cnt = 0;
        DMA[3].src = &image[OFFSET(row, 0, width)];
        DMA[3].dst = &videoBuffer[OFFSET(y + row, x, WIDTH)];

        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_ON;
    }
}

// Fill the screen with a solid color
//
// color: color to use
void fillScreenDMA(volatile u16 color)
{
    DMA[3].cnt = 0;
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = WIDTH * HEIGHT | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_ON;
}

// Draw a character to screen
//
// c: column to start drawing the char at
// r: row to start drawing the char at
// ch: character to draw
// color: color to use
void drawChar(int col, int row, char ch, u16 color)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 6; c++)
        {
            if (fontdata_6x8[OFFSET(r, c, 6) + ch * 48])
            {
                setPixel(col + c, row + r, color);
            }
        }
    }
}

// Draw a string to screen
//
// c: column to start drawing the string at
// r: row to start drawing the string at
// str: pointer to string to draw
// color: color to use
void drawString(int col, int row, char *str, u16 color)
{
    while (*str)
    {
        drawChar(col, row, *str++, color);
        col += 6;
    }
}

// Draw a string centered to coordinates
//
// x: x position string to be centered at
// y: y position string to be centered at
// width: width of string
// height: height of string
// str: pointer to string to draw
// color: color to use
void drawCenteredString(int x, int y, int width, int height, char *str, u16 color)
{
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy)
    {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}
