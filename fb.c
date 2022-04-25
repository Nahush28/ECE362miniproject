#include <stdint.h>
#include <string.h>
#include "fb.h"

const uint8_t baseStageHeight = LED_V_PX - 1;

void clearFb(fb fbuf)
{
    memset(fbuf, 0, FB_SIZE);
}

uint8_t readPx(fb fbuf, uint32_t x, uint32_t y)
{
    uint32_t idx = FBIDX(x, y);
    uint32_t offset = idx % BIT_SIZE;
    idx /= BIT_SIZE;
    return (fbuf[idx] >> offset) & CLOR_MASK;
}

void writePx(fb fbuf, uint8_t clor, uint32_t x, uint32_t y)
{
    uint32_t idx = FBIDX(x, y);
    uint32_t offset = idx % BIT_SIZE;
    idx /= BIT_SIZE;
    clor &= CLOR_MASK;
    fbuf[idx] &= ~(CLOR_MASK << offset);
    fbuf[idx] |= (clor << offset);
}

uint8_t* readRow(fb fbuf, uint32_t row)
{
    return fbuf + row * ROW_SIZE;
}

void scrollFrameL(fb fbuf, uint32_t n)
{
    n *= CLOR_BIT;
    // whole uint8 moving
    if(n >= BIT_SIZE) {
        uint32_t jmpSize = n / BIT_SIZE;
        for(int i = 0; i < LED_V_PX; i++) {
            int j;
            for(j = 0; j < ROW_SIZE - jmpSize; j++) {
                fbuf[i * ROW_SIZE + j] = fbuf[i * ROW_SIZE + j + jmpSize];
            }
            for(; j < ROW_SIZE; j++) {
                fbuf[i * LED_H_PX * CLOR_BIT / BIT_SIZE + j] = 0;
            }
        }
    }
    // move 4 bits
    if(n % BIT_SIZE) {
        for(int i = 0; i < LED_V_PX; i++) {
            uint8_t movBuf = fbuf[i * ROW_SIZE + 1] & CLOR_MASK;
            for(int j = 0; j < ROW_SIZE - 1; j++) {
                fbuf[i * ROW_SIZE + j] = (fbuf[i * ROW_SIZE + j] >> CLOR_BIT) | (movBuf << (BIT_SIZE - CLOR_BIT));
                movBuf = fbuf[i * ROW_SIZE + j + 2] & CLOR_MASK;
            }
            fbuf[(i + 1) * ROW_SIZE - 1] = fbuf[(i + 1) * ROW_SIZE - 1] >> CLOR_BIT;

        }
    }
}

uint8_t oldStgHeight = 65;

void drawStage(fb fbuf, uint32_t stageHeight, uint32_t scrollNum)
{
    if(oldStgHeight == 65)
        oldStgHeight = stageHeight;
    for(int i = LED_H_PX - scrollNum; i < LED_H_PX; i++)
        writePx(fbuf, R, stageHeight, i);
    if(oldStgHeight != stageHeight) {
        for(int i = MIN(stageHeight, oldStgHeight); i <= MAX(stageHeight, oldStgHeight); i++)
            writePx(fbuf, R, i, LED_H_PX - scrollNum - 1);
    }
    oldStgHeight = stageHeight;
}

static uint8_t readSprite(Sprite* sprite, uint32_t x, uint32_t y)
{
    uint32_t idx = (x * sprite->ySize + y) * CLOR_BIT;
    uint32_t offset = idx % BIT_SIZE;
    idx /= BIT_SIZE;
    return ((sprite->pxMap)[idx] >> offset) & CLOR_MASK;
}

// The upper left corner of sprite is at (x, y)
// If collision, return 1, else 0
uint8_t drawSprite(fb fbuf, Sprite* sprite, int32_t x, int32_t y)
{
    for(int i = 0; i < sprite->xSize && i + x < LED_V_PX; i++) {
        for(int j = 0; j < sprite->ySize && j + y < LED_H_PX; j++) {
            if(readPx(fbuf, i + x, j + y))
                return 1;
            writePx(fbuf, readSprite(sprite, i, j), i + x, j + y);
        }
    }
    // Collision checking
    // Check bottom left corner for now
    if(readPx(fbuf, x + sprite->xSize - 1, y + sprite->ySize))
        return 1;
    return 0;
}

void clearSprite(fb fbuf, Sprite* sprite, uint32_t x, uint32_t y)
{
    for(int i = x; i < x + sprite->xSize && i < LED_V_PX; i++) {
        for(int j = y; j < y + sprite->ySize && j < LED_H_PX; j++) {
            writePx(fbuf, K, i, j);
        }
    }
}

