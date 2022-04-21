#include "stm32f0xx.h"
#include "fb.h"

#define SCROLL_AMOUNT 2

struct SpriteLoc {
	uint8_t x,
	uint8_t y
};

int main(void)
{
	// init
	fb fbuf;
	struct SpriteLoc playerLoc = {.x = NUM_ROWS - player.xSize - 1; .y = 0};
	uint8_t gameState;

	clearFb(fbuf);
	drawStage(fbuf, NUM_ROWS - 1, LED_H_PX);
	drawSprite(fbuf, &player, playerLoc.x, playerLoc.y);
	for(;;) {
		// Refresh led here

		clearSprite(fbuf, &player, playerLoc.x, playerLoc.y);
		scrollFrameL(fbuf, SCROLL_AMOUNT);
		// Do all your game logic here

		if(drawSprite(fbuf, &player, playerLoc.x, playerLoc.y)) {
			// Lose
		}
	}
}