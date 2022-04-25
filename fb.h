#ifndef __FB_H__
#define __FB_H__

#include <stdint.h>

/*
low addr -----------> high addr
-----------------------------------------
|xxxx xxxx|xxxx xxxx|xxxx xxxx| ...     |
|...      |         |         |         | 32 lines
-----------------------------------------
64 cols * 4 bits per px

low -> high
x x x x
B G R 0
*/

#define LED_H_PX 64
#define LED_V_PX 32
#define NUM_ROWS LED_V_PX
#define NUM_COLS LED_H_PX
// top 1 bit unused
#define CLOR_BIT 4
#define CLOR_MASK 7
#define BIT_SIZE 8
#define FB_SIZE ROUNDUP(LED_H_PX * LED_V_PX * CLOR_BIT / BIT_SIZE, BIT_SIZE)
#define ROW_SIZE (LED_H_PX * CLOR_BIT / BIT_SIZE)

#define ROUNDDOWN(num, unit) ((num) - (num) % (unit))
#define ROUNDUP(num, unit) (ROUNDDOWN((num) + (unit) - 1, (unit)))
#define MAX(n, m) ((n) > (m) ? (n) : (m))
#define MIN(n, m) ((n) < (m) ? (n) : (m))
// gives bit position
#define FBIDX(x, y) (((x) * LED_H_PX + (y)) * CLOR_BIT)
#define SPRITEPX(x, y) (x | (y << CLOR_BIT))

// Colors
#define K 0
#define B 1
#define G 2
#define C 3 // Cyan
#define R 4
#define M 5 // magenta
#define Y 6 // yellow
#define W 7

typedef uint8_t fb[FB_SIZE];
typedef struct __sprite {
    uint8_t pxMap[FB_SIZE];
    uint8_t xSize;
    uint8_t ySize;
} Sprite;

void clearFb(fb fbuf);
uint8_t readPx(fb fbuf, uint32_t x, uint32_t y);
void writePx(fb fbuf, uint8_t clor, uint32_t x, uint32_t y);
uint8_t* readRow(fb fbuf, uint32_t row);
void scrollFrameL(fb fbuf, uint32_t n);
void drawStage(fb fbuf, uint32_t stageHeight, uint32_t scrollNum);
uint8_t drawSprite(fb fbuf, Sprite* sprite, int32_t x, int32_t y);
void clearSprite(fb fbuf, Sprite* sprite, uint32_t x, uint32_t y);

#endif
