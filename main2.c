#include "stm32f0xx.h"
#include "fb.h"

#define SCROLL_AMOUNT 2
#define PLAYER_Y 0
#define JUMP_V0 6
#define GRAVITY 2
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

static uint8_t getRand()
{
	return 0;
}

void drawScore()
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
}

uint8_t isFallThrough(fb fbuf, uint8_t oldX, uint8_t newX)
{
	return 0;
}

int main(void)
{
	// init
	fb fbuf;
	uint8_t gameState = GAME_INIT;
	int8_t playerX = LED_V_PX - player.xSize - 1;
	int8_t playerVSpd = 0;
	uint8_t blankPeriod = 0;
	uint8_t stageHeight = NUM_ROWS - 1;
	uint8_t curStgHeight = 0;

	clearFb(fbuf);
	drawStage(fbuf, stageHeight, LED_H_PX);
	drawSprite(fbuf, &player, playerX, PLAYER_Y);
	while(1) {
		// Refresh led here
		translate_frame(fbuf);
		draw_bitmap();
		// Game logic
		switch(gameState) {
		case GAME_INIT:
			if(pressed) {
				pressed = 0;
				gameState = GAME_RUNNING;
			}
			break;
		case GAME_RUNNING:
			if(pressed) {
				pressed = 0;
				playerVSpd = JUMP_V0;
				gameState = GAME_JUMPING;
			} else if(!readPx(fbuf, playerX - player.xSize, PLAYER_Y)) {
				gameState = GAME_JUMPING;
			}
			// fall off detection here
		case GAME_JUMPING:
			pressed = 0;
			clearSprite(fbuf, &player, playerX, PLAYER_Y);
			// Fall to stage detection
			if(curStgHeight = isFallThrough(fbuf, playerX, playerX - playerVSpd)) {
				playerX = curStgHeight;
				playerVSpd = 0;
				gameState = GAME_RUNNING;
			} else {
				playerX -= playerVSpd;
				playerVSpd = playerVSpd != 0 ? playerVSpd - GRAVITY : 0;
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
			if(drawSprite(fbuf, &player, playerX, PLAYER_Y)) {
				gameState = GAME_DEAD;
				// Draw dead screen here
			}
			break;
		case GAME_DEAD:
			if(is_Jumping) {
				is_Jumping = 0;
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