#include <stdio.h>
#include "fb.h"

#define ANSI_RST "\x1b[40m"
#define ANSI_R "\x1b[41m"
#define ANSI_G "\x1b[42m"
#define ANSI_Y "\x1b[43m"
#define ANSI_B "\x1b[44m"
#define ANSI_M "\x1b[45m"
#define ANSI_C "\x1b[46m"
#define ANSI_W "\x1b[47m"

void drawFrame(fb);

extern Sprite player;

int main(void)
{
	fb fbuf;
	// Single pixel drawing
	clearFb(fbuf);
	for(int i = 0; i < LED_H_PX; i++) {
		writePx(fbuf, (i + 1) % CLOR_MASK, 1, i);
	}
	drawFrame(fbuf);
	scrollFrameL(fbuf, 1);
	drawFrame(fbuf);

	// Stage drawing, scrolling
	clearFb(fbuf);
	drawStage(fbuf, LED_V_PX - 1, LED_H_PX);
	drawFrame(fbuf);
	scrollFrameL(fbuf, 3);
	drawStage(fbuf, LED_V_PX - 3, 3);
	drawFrame(fbuf);
	scrollFrameL(fbuf, 1);
	drawStage(fbuf, LED_V_PX - 3, 1);
	drawFrame(fbuf);

	// Sprites
	drawSprite(fbuf, &player, LED_V_PX - 4, 0);
	drawFrame(fbuf);
	clearSprite(fbuf, &player, LED_V_PX - 4, 0);
	drawFrame(fbuf);
	return 0;
}

void drawFrame(fb fbuf)
{
	for(int i = 0; i < LED_V_PX; i++) {
		for(int j = 0; j < LED_H_PX; j++) {
			switch (readPx(fbuf, i, j)) {
			case 0:
				printf(ANSI_RST);
				printf(" ");
				break;
			case 1:
				printf(ANSI_B);
				printf(" ");
				break;
			case 2:
				printf(ANSI_G);
				printf(" ");
				break;
			case 3:
				printf(ANSI_C);
				printf(" ");
				break;
			case 4:
				printf(ANSI_R);
				printf(" ");
				break;
			case 5:
				printf(ANSI_M);
				printf(" ");
				break;
			case 6:
				printf(ANSI_Y);
				printf(" ");
				break;
			case 7:
				printf(ANSI_W);
				printf(" ");
				break;
			}
		}
		printf(ANSI_RST);
		printf("\n");
	}
	printf("Frame ends\n");
}