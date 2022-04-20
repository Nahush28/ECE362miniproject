#include <stdint.h>
#include <string.h>
#include "fb.h"

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
		for(int i = 0; i < LED_V_PX - 1; i++) {
			uint8_t movBuf = fbuf[i * ROW_SIZE + 1] & CLOR_MASK;
			for(int j = 0; j < ROW_SIZE - 1; j++) {
				fbuf[i * ROW_SIZE + j] = (fbuf[i * ROW_SIZE + j] >> CLOR_BIT) | (movBuf << (BIT_SIZE - CLOR_BIT));
				movBuf = fbuf[i * ROW_SIZE + j + 2] & CLOR_MASK;
			}
			fbuf[(i + 1) * ROW_SIZE - 1] = fbuf[(i + 1) * ROW_SIZE - 1] >> CLOR_BIT;
		}
	}
}
