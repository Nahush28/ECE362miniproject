#include "stm32f0xx.h"
#include "fb.h"

#define SCROLL_AMOUNT 2
#define PLAYER_Y 0
#define JUMP_V0 6
#define GRAVITY 1
#define MIN_OBST_DIST 6
#define STG_HEIGHT_INC 6
#define MAX_STAGE_HEIGHT 6

enum {
	GAME_INIT = 0,
	GAME_RUNNING,
	GAME_JUMPING,
	GAME_DEAD
};

enum {
	GENERATE_FLAT = 0,
	GENERATE_OBST,
	GENERATE_STAGE,
	GENERATE_DESTAGE,
	GENERATE_VAR0,
	GENERATE_VAR1,
	GENERATE_VAR2,
	GENERATE_VAR3
};

extern uint8_t pressed;
extern uint8_t bitmap[16][64];
extern uint8_t start_screen[1024];
extern uint8_t end_screen[1024];

extern Sprite player, obstacle;

static uint8_t getRand()
{
	return 0;
}

/*void drawScore()
{
	static check_score = 0;
	if (check_score == 25) {
                dig1++;
                if (dig1 == 10) {
                    dig1 = 0;
                    dig2++;
                }
                if (dig2 == 10) {
                    dig2 = 0;
                    dig3++;
                }
                if (dig3 == 10) {
                    dig3 = 0;
                    dig4++;
                }
                if (dig4 == 10) {
                    dig4 = 0;
                }
                check_score = 0;
        } else
                check_score++;

        draw_digit(2, 57, dig1, 1);
        if(dig2 || dig3 || dig4)
        	draw_digit(2, 51, dig2, 1);
        if(dig3 || dig4)
        	draw_digit(2, 45, dig3, 1);
        if(dig4)
        	draw_digit(2, 38, dig4, 1);
}*/

uint8_t isFallThrough(fb fbuf, uint8_t oldX, uint8_t newX)
{
	for(int i = oldX + player.xSize; i < MIN(newX + player.xSize, LED_V_PX); i++) {
		if(readPx(fbuf, i, PLAYER_Y))
			return i;
		else if(i == LED_V_PX)
			return LED_V_PX - 1;
	}
	return 0;
}

void translateFB(fb fbuf)
{
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 64; j++) {
			bitmap[i][j] = (readPx(fbuf, i, j) << 4) | readPx(fbuf, i + 16, j);
		}
	}
}

extern Sprite player;

uint8_t slowDown = 0;

int main(void)
{
	// init
	fb fbuf;
	uint8_t gameState = GAME_RUNNING;
	int8_t playerX = LED_V_PX - player.xSize - 1;
	int8_t playerVSpd = 0;
	uint8_t blankPeriod = 0;
	uint8_t stageHeight = NUM_ROWS - 1;
	uint8_t curStgHeight = 0;

	enable_ports();
	enable_exti();

	clearFb(fbuf);
	while(1) {
		if(slowDown != 75) {
			slowDown++;
			continue;
		}
		else
			slowDown = 0;
		// Refresh led here
		translateFB(fbuf);
		if(gameState == GAME_INIT) {
			memcpy(bitmap, start_screen, 16 * 64);
		} else if(gameState == GAME_DEAD) {
			memcpy(bitmap, end_screen, 16 * 64);
		}
		draw_bitmap();
		// Game logic
		switch(gameState) {
		case GAME_INIT:
			if(pressed) {
				pressed = 0;
				gameState = GAME_RUNNING;
				drawStage(fbuf, stageHeight, LED_H_PX);
				drawSprite(fbuf, &player, playerX, PLAYER_Y);
			}
			break;
		case GAME_RUNNING:
			if(pressed) {
				pressed = 0;
				playerVSpd = JUMP_V0;
				gameState = GAME_JUMPING;
			} else if(!readPx(fbuf, playerX - player.xSize, PLAYER_Y)) {
				// Fall off
				playerVSpd = -1;
				gameState = GAME_JUMPING;
			}
		case GAME_JUMPING:
			pressed = 0;
			clearSprite(fbuf, &player, playerX, PLAYER_Y);
			// Fall to stage detection
			if(curStgHeight = isFallThrough(fbuf, playerX, playerX - playerVSpd)) {
				playerX = curStgHeight - player.xSize;
				playerVSpd = 0;
				gameState = GAME_RUNNING;
			} else {
				playerX -= playerVSpd;
				if(gameState == GAME_JUMPING)
					playerVSpd -= GRAVITY;
			}
			scrollFrameL(fbuf, SCROLL_AMOUNT);

			blankPeriod = blankPeriod == 0 ? 0 : blankPeriod - SCROLL_AMOUNT;
			// Generation
			switch(getRand()) {
			case GENERATE_OBST:
				if(!blankPeriod) {
					// draw obstacle here
					blankPeriod = MIN_OBST_DIST;
				}
				break;
			case GENERATE_STAGE:
				if(!blankPeriod) {
					if(stageHeight > MAX_STAGE_HEIGHT)
						stageHeight -= STG_HEIGHT_INC;
					else
						stageHeight += STG_HEIGHT_INC;
					blankPeriod = MIN_OBST_DIST;
				}
				break;
			case GENERATE_DESTAGE:
				if(!blankPeriod) {
					stageHeight += STG_HEIGHT_INC;
					blankPeriod = MIN_OBST_DIST;
				}
			case GENERATE_FLAT:
			default:
				break;
			}

			drawStage(fbuf, stageHeight, SCROLL_AMOUNT);
			drawSprite(fbuf, &player, playerX, PLAYER_Y);
			/*if(drawSprite(fbuf, &player, playerX, PLAYER_Y)) {
				gameState = GAME_DEAD;
			}*/
			break;
		case GAME_DEAD:
			if(pressed) {
				pressed = 0;
				clearFb(fbuf);
				playerX = LED_V_PX - player.xSize - 1;
				playerVSpd = 0;
				blankPeriod = 0;
				stageHeight = NUM_ROWS - 1;
				drawStage(fbuf, stageHeight, LED_H_PX);
				drawSprite(fbuf, &player, playerX, PLAYER_Y);
				gameState = GAME_RUNNING;
			}
			break;
		}
	}
}