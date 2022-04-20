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
*/

#define LED_H_PX 64
#define LED_V_PX 32
// top 1 bit unused
#define CLOR_BIT 4
#define CLOR_MASK 7
#define BIT_SIZE 8
#define FB_SIZE ROUNDUP(LED_H_PX * LED_V_PX * CLOR_BIT / BIT_SIZE, BIT_SIZE)
#define ROW_SIZE (LED_H_PX * CLOR_BIT / BIT_SIZE)

#define ROUNDDOWN(num, unit) ((num) - (num) % (unit))
#define ROUNDUP(num, unit) (ROUNDDOWN((num) + (unit) - 1, (unit)))
// gives bit position
#define FBIDX(x, y) (((x) * LED_H_PX + (y)) * CLOR_BIT)

typedef uint8_t fb[FB_SIZE];

void clearFb(fb);
uint8_t readPx(fb, uint32_t, uint32_t);
void writePx(fb, uint8_t, uint32_t, uint32_t);
uint8_t* readRow(fb, uint32_t);
void scrollFrameL(fb, uint32_t);

#endif
