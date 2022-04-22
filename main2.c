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
	GENERATE_BLCK_STAGE,
	GENERATE_STAGE,
	GENERATE_DESTAGE,
	GENERATE_VAR0,
	GENERATE_VAR1,
	GENERATE_VAR2
};

uint8_t is_Jumping;

static uint8_t getRand()
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

	clearFb(fbuf);
	drawStage(fbuf, stageHeight, LED_H_PX);
	drawSprite(fbuf, &player, playerX, PLAYER_Y);
	while(1) {
		// Refresh led here

		// Game logic
		switch(gameState) {
		case GAME_INIT:
			if(is_Jumping) {
				is_Jumping = 0;
				gameState = GAME_RUNNING;
			}
			break;
		case GAME_RUNNING:
			if(is_Jumping) {
				is_Jumping = 0;
				playerVSpd = JUMP_V0;
				gameState = GAME_JUMPING;
			}
		case GAME_JUMPING:
			is_Jumping = 0;
			clearSprite(fbuf, &player, playerX, PLAYER_Y);
			playerX += -playerVSpd; // Well, smaller x is higher on frame, I blame the arr for that
			playerVSpd = playerVSpd != 0 ? playerVSpd - GRAVITY : 0;
			if(playerX >= stageHeight) {
				playerX = stageHeight;
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
			case GENERATE_BLCK_STAGE:
				if(!blankPeriod) {
					// draw obstacle here
					blankPeriod = MIN_OBST_DIST;
				}
			case GENERATE_STAGE:
				if(stageHeight > MAX_STAGE_HEIGHT)
					stageHeight -= STG_HEIGHT_INC;
				else
					stageHeight += STG_HEIGHT_INC;
				blankPeriod = MIN_OBST_DIST;
				break;
			case GENERATE_DESTAGE:
				stageHeight += STG_HEIGHT_INC;
				blankPeriod = MIN_OBST_DIST;
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