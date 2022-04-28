#include "stm32f0xx.h"
#include "fb.h"

#include "midi.h"
#include "midiplay.h"
#include "musicplayer.h"

//#define SCROLL_AMOUNT 1
#define PLAYER_Y 2 //horizontal
#define JUMP_V0 5
#define GRAVITY 1
//#define MIN_OBST_DIST 32
#define STG_HEIGHT_INC 6
#define MAX_STAGE_HEIGHT 18
#define OBST_SPAWN_X (stageHeight - obstacle.xSize)
#define OBST_SPAWN_Y (LED_H_PX - obstacle.ySize - 1)
#define STG_2_TRESH 350
#define STG_3_TRESH 850
#define SCORE ((dig1 + 10 * dig2 + 100 * dig3 + 1000 * dig4))

uint8_t SCROLL_AMOUNT = 1;
uint8_t MIN_OBST_DIST = 32;

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
};

extern uint8_t pressed;
extern uint8_t bitmap[16][64];
extern uint8_t start_screen[1024];
extern uint8_t end_screen[1024];
extern uint8_t d_0[35];
extern uint8_t d_1[35];
extern uint8_t d_2[35];
extern uint8_t d_3[35];
extern uint8_t d_4[35];
extern uint8_t d_5[35];
extern uint8_t d_6[35];
extern uint8_t d_7[35];
extern uint8_t d_8[35];
extern uint8_t d_9[35];
extern uint8_t dig1;
extern uint8_t dig2;
extern uint8_t dig3;
extern uint8_t dig4;
extern const uint8_t midifile2[];

extern Sprite player, obstacle;

static uint8_t getRand()
{
    if((SCORE > STG_2_TRESH - 30 && SCORE < STG_2_TRESH) || (SCORE > STG_3_TRESH - 50 && SCORE < STG_3_TRESH))
    {
        return GENERATE_FLAT;
    }
    srand(TIM6->CNT);
    return rand() % 4;
}

uint8_t scoreSlowDown = 0;

void updateScore() {
    if(SCORE == STG_2_TRESH) {
        SCROLL_AMOUNT = 2;
    } else if(SCORE == STG_3_TRESH) {
        SCROLL_AMOUNT = 3;
        MIN_OBST_DIST += 16;
    }
    if (scoreSlowDown == 1) {
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
        scoreSlowDown = 0;
    } else
        scoreSlowDown++;
}

void drawScore()
{
    draw_digit(2, 57, dig1, 1);
    if(dig2 || dig3 || dig4)
        draw_digit(2, 51, dig2, 1);
    if(dig3 || dig4)
        draw_digit(2, 45, dig3, 1);
    if(dig4)
        draw_digit(2, 38, dig4, 1);
}

static uint8_t isFallThrough(fb fbuf, int8_t oldX, int8_t newX)
{
    for(int i = oldX + player.xSize; i < MIN(newX + player.xSize, LED_V_PX); i++) {
        if(readPx(fbuf, i, PLAYER_Y) == R)
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

extern Sprite player, obstacle;

extern uint8_t oldStgHeight;

uint8_t slowDown = 0;


int main(void)
{
    // init
    fb fbuf;
    uint8_t gameState = GAME_INIT;
    int8_t playerX = LED_V_PX - player.xSize - 1;
    int8_t playerVSpd = 0;
    int8_t blankPeriod = 0;
    uint8_t stageHeight = NUM_ROWS - 1;
    uint8_t curStgHeight = 0;

    enable_ports();
    enable_exti();

    //Music stuff:
    init_wavetable_all();
    init_musicplayer_peripherals();

    reset_music();
    MIDI_Player *mp = midi_init(midifile2);
    init_tim2(10417);


    clearFb(fbuf);
    while(1) {
        check_music(mp);

        if(slowDown != 100) {
            slowDown++;
            continue;
        }
        else
            slowDown = 0;
        // Refresh led here
        translateFB(fbuf);

        drawScore();

        if(gameState == GAME_INIT) {
            memcpy(bitmap, start_screen, 16 * 64);
        } else if(gameState == GAME_DEAD) {
            memcpy(bitmap, end_screen, 16 * 64);

            draw_digit(7, 39, dig1, 0);
            draw_digit(7, 33, dig2, 0);
            draw_digit(7, 27, dig3, 0);
            draw_digit(7, 20, dig4, 0);
        }
        draw_bitmap();
        // Game logic
        switch(gameState) {
        case GAME_INIT:

            if(pressed) {
                pressed = 0;
                gameState = GAME_RUNNING;

                reset_music();
                mp = midi_init(midifile);
                init_tim2(10417);

                drawStage(fbuf, stageHeight, LED_H_PX);
                drawSprite(fbuf, &player, playerX, PLAYER_Y);
            }
            break;
        case GAME_RUNNING:
            if(pressed) {
                pressed = 0;
                playerVSpd = JUMP_V0;
                gameState = GAME_JUMPING;
            } else if(!readPx(fbuf, playerX + player.xSize, PLAYER_Y)) {
                // Fall off
                gameState = GAME_JUMPING;
            }
        case GAME_JUMPING:
            updateScore();
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

            blankPeriod = blankPeriod <= 0 ? 0 : blankPeriod - SCROLL_AMOUNT;
            // Generation
            switch(getRand()) {
            case GENERATE_OBST:
                if(blankPeriod == 0) {
                    // draw obstacle here
                    drawSprite(fbuf, &obstacle, OBST_SPAWN_X, OBST_SPAWN_Y);
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
                if(!blankPeriod && stageHeight > NUM_ROWS - 1) {
                    stageHeight += STG_HEIGHT_INC;
                    blankPeriod = MIN_OBST_DIST;
                }
            default:
                break;
            }

            drawStage(fbuf, stageHeight, SCROLL_AMOUNT);
            if(drawSprite(fbuf, &player, playerX, PLAYER_Y)) {
                gameState = GAME_DEAD;
            }
            break;
        case GAME_DEAD:
            reset_music();

            if(pressed) {
                dig1 = 0;
                dig2 = 0;
                dig3 = 0;
                dig4 = 0;


                SCROLL_AMOUNT = 1;
                MIN_OBST_DIST = 32;
                pressed = 0;
                clearFb(fbuf);
                oldStgHeight = 65;
                playerX = LED_V_PX - player.xSize - 1;
                playerVSpd = 0;
                blankPeriod = 0;
                stageHeight = NUM_ROWS - 1;
                drawStage(fbuf, stageHeight, LED_H_PX);
                drawSprite(fbuf, &player, playerX, PLAYER_Y);
                gameState = GAME_INIT;

                mp = midi_init(midifile);
            }

            break;
        }
    }
}
