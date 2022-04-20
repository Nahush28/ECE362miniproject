/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"

#include <stdio.h>
			

void enable_ports(){
    /**
     * GPIOA - mapped to the Clock (CLK) and output enable (OE)
     * GPIOB - mapped to the colors R1 R2 B1 B2 G1 G2 And latch (LAT)
     * GPIOC - mapped to the row select lines (A B C D)
     */

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
//    GPIOA->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_0;
    GPIOA->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0; // PA8 - CLK, PA9-OE
    GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0;
    //PB8-R1, PB9-R2, PB10-B1, PB11-B2, PB12-G1, PB13-G2, PB4-LAT
    GPIOC->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0| GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;
    //PC0-A, PC1-B, PC2-C, PC3-D

//    GPIOA->AFR[1] |= 0x2;
}

void set_row(uint8_t RGB, int row, int cur){ //currently can only handle one color at a time
  row--; //so you don't have to think too hard :)
  if(row == cur){
    if(row < 16){
        if(RGB == 0b001){ //if blue
            GPIOB->BSRR |= 1<<10;
        }else if(RGB == 0b010){ //if green
            GPIOB->BSRR |= 1<<12;
        }else if(RGB == 4){ //if red
            GPIOB->BSRR |= 1<<8;
        }else if(RGB == 5){ //if red-blue
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<8;
        }else if(RGB == 6){ //if red-green
            GPIOB->BSRR |= 1<<8;
            GPIOB->BSRR |= 1<<12;
        }else if(RGB == 3){ // if green-blue
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<10;
        }else if(RGB == 7){ //if white
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<8;
        }
    }else{
        if(RGB == 0b001){ //if blue
            GPIOB->BSRR |= 1<<11;
        }else if(RGB == 0b010){ //if green
            GPIOB->BSRR |= 1<<13;
        }else if(RGB == 4){ //if red
            GPIOB->BSRR |= 1<<9;
        }else if(RGB == 5){ //if red-blue
            GPIOB->BSRR |= 1<<11;
            GPIOB->BSRR |= 1<<9;
        }else if(RGB == 6){ //if red-green
            GPIOB->BSRR |= 1<<9;
            GPIOB->BSRR |= 1<<13;
        }else if(RGB == 3){ // if green-blue
            GPIOB->BSRR |= 1<<13;
            GPIOB->BSRR |= 1<<11;
        }else if(RGB == 7){ //if white
            GPIOB->BSRR |= 1<<13;
            GPIOB->BSRR |= 1<<11;
            GPIOB->BSRR |= 1<<9;
        }
    }
  }else{
      if(row < 16){
          if(RGB == 0b001){ //if blue
              GPIOB->BRR |= 1<<10;
          }else if(RGB == 0b010){ //if green
              GPIOB->BRR |= 1<<12;
          }else if(RGB == 4){ //if red
              GPIOB->BRR |= 1<<8;
          }else if(RGB == 5){ //if red-blue
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<8;
          }else if(RGB == 6){ //if red-green
              GPIOB->BRR |= 1<<8;
              GPIOB->BRR |= 1<<12;
          }else if(RGB == 3){ // if green-blue
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<10;
          }else if(RGB == 7){ //if white
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<8;
          }
      }else{
          if(RGB == 0b001){ //if blue
              GPIOB->BRR |= 1<<11;
          }else if(RGB == 0b010){ //if green
              GPIOB->BRR |= 1<<13;
          }else if(RGB == 4){ //if red
              GPIOB->BRR |= 1<<9;
          }else if(RGB == 5){ //if red-blue
              GPIOB->BRR |= 1<<11;
              GPIOB->BRR |= 1<<9;
          }else if(RGB == 6){ //if red-green
              GPIOB->BRR |= 1<<9;
              GPIOB->BRR |= 1<<13;
          }else if(RGB == 3){ // if green-blue
              GPIOB->BRR |= 1<<13;
              GPIOB->BRR |= 1<<11;
          }else if(RGB == 7){ //if white
              GPIOB->BRR |= 1<<13;
              GPIOB->BRR |= 1<<11;
              GPIOB->BRR |= 1<<9;
          }
      }
  }
}

//void nano_wait(unsigned int n) {
//    asm(    "        mov r0,%0\n"
//            "repeat: sub r0,#83\n"
//            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
//}
//
//void set_row(uint8_t RGB, int row, int cur){ //currently can only handle one color at a time
//  row--; //so you don't have to think too hard :)
//  unsigned int n = 10000;
//  if(row == cur){
//    if(row < 16){
//        if(RGB == 0b001){ //if blue
//            GPIOB->BSRR |= 1<<10;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<10;
//        }else if(RGB == 0b010){ //if green
//            GPIOB->BSRR |= 1<<12;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<12;
//        }else if(RGB == 4){ //if red
//            GPIOB->BSRR |= 1<<8;
//           nano_wait(n);
//            GPIOB->BRR |= 1<<8;
//        }else if(RGB == 5){ //if red-blue
//            GPIOB->BSRR |= 1<<10;
//            GPIOB->BSRR |= 1<<8;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<10;
//            GPIOB->BRR |= 1<<8;
//        }else if(RGB == 6){ //if red-green
//            GPIOB->BSRR |= 1<<8;
//            GPIOB->BSRR |= 1<<12;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<8;
//            GPIOB->BRR |= 1<<12;
//        }else if(RGB == 3){ // if green-blue
//            GPIOB->BSRR |= 1<<12;
//            GPIOB->BSRR |= 1<<10;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<12;
//            GPIOB->BRR |= 1<<10;
//        }else if(RGB == 7){ //if white
//            GPIOB->BSRR |= 1<<12;
//            GPIOB->BSRR |= 1<<10;
//            GPIOB->BSRR |= 1<<8;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<12;
//            GPIOB->BRR |= 1<<10;
//            GPIOB->BRR |= 1<<8;
//        }
//    }else{
//        if(RGB == 0b001){ //if blue
//            GPIOB->BSRR |= 1<<11;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<11;
//        }else if(RGB == 0b010){ //if green
//            GPIOB->BSRR |= 1<<13;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<13;
//        }else if(RGB == 4){ //if red
//            GPIOB->BSRR |= 1<<9;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<9;
//        }else if(RGB == 5){ //if red-blue
//            GPIOB->BSRR |= 1<<11;
//            GPIOB->BSRR |= 1<<9;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<11;
//            GPIOB->BRR |= 1<<9;
//        }else if(RGB == 6){ //if red-green
//            GPIOB->BSRR |= 1<<9;
//            GPIOB->BSRR |= 1<<13;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<9;
//            GPIOB->BRR |= 1<<13;
//        }else if(RGB == 3){ // if green-blue
//            GPIOB->BSRR |= 1<<13;
//            GPIOB->BSRR |= 1<<11;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<13;
//            GPIOB->BRR |= 1<<11;
//        }else if(RGB == 7){ //if white
//            GPIOB->BSRR |= 1<<13;
//            GPIOB->BSRR |= 1<<11;
//            GPIOB->BSRR |= 1<<9;
//            nano_wait(n);
//            GPIOB->BRR |= 1<<13;
//            GPIOB->BRR |= 1<<11;
//            GPIOB->BRR |= 1<<9;
//        }
//    }
//  }
//}

void set_col(uint8_t RGB, int col, int cur){
  col--; //so you don't have to think too hard :)
  if(col == cur){
        if(RGB == 0b001){ //if blue
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<11;
        }else if(RGB == 0b010){ //if green
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<13;
        }else if(RGB == 4){ //if red
            GPIOB->BSRR |= 1<<8;
            GPIOB->BSRR |= 1<<9;
        }else if(RGB == 5){ //if red-blue
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<8;
            GPIOB->BSRR |= 1<<11;
            GPIOB->BSRR |= 1<<9;
        }else if(RGB == 6){ //if red-green
            GPIOB->BSRR |= 1<<8;
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<9;
            GPIOB->BSRR |= 1<<13;
        }else if(RGB == 3){ // if green-blue
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<13;
            GPIOB->BSRR |= 1<<11;
        }else if(RGB == 7){ //if white
            GPIOB->BSRR |= 1<<12;
            GPIOB->BSRR |= 1<<10;
            GPIOB->BSRR |= 1<<8;
            GPIOB->BSRR |= 1<<13;
            GPIOB->BSRR |= 1<<11;
            GPIOB->BSRR |= 1<<9;
        }
  }else{
          if(RGB == 0b001){ //if blue
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<11;
          }else if(RGB == 0b010){ //if green
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<13;
          }else if(RGB == 4){ //if red
              GPIOB->BRR |= 1<<8;
              GPIOB->BRR |= 1<<9;
          }else if(RGB == 5){ //if red-blue
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<8;
              GPIOB->BRR |= 1<<11;
              GPIOB->BRR |= 1<<9;
          }else if(RGB == 6){ //if red-green
              GPIOB->BRR |= 1<<8;
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<9;
              GPIOB->BRR |= 1<<13;
          }else if(RGB == 3){ // if green-blue
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<13;
              GPIOB->BRR |= 1<<11;
          }else if(RGB == 7){ //if white
              GPIOB->BRR |= 1<<12;
              GPIOB->BRR |= 1<<10;
              GPIOB->BRR |= 1<<8;
              GPIOB->BRR |= 1<<13;
              GPIOB->BRR |= 1<<11;
              GPIOB->BRR |= 1<<9;
          }
  }
}

int main(void)
{

    /**
     * enables all the ports that are to be used to run the peripheral. More
     * details inside the function.
     */
    enable_ports();


    /*this infinite for loop is for driving the matrix. We can only display one
     * row at a time so we need to drive the display fast enough for them all
     * to appear visible
    */
    for(;;){
    for(int row = 0; row < 32; row++){ // 32 rows but every for every row select you can drive 2 rows.
        for(int col = 0; col < 64; col++){ // need to clock 64 times for each column on the display
//            if(col == 32) // simple if condition which says turn the upper current pixel green if its not the middle most one
//                GPIOB->BSRR |= 1<<10;
//            else
//                GPIOB->BRR |= 1<<10;
//
//            col_RGB(0x0000010, 15, j);


                            //0bRGB  line# row/column
            set_row((uint8_t)(0b100), 32, row);
            set_row((uint8_t)(0b110), 10, row);
            //set_row((uint8_t)(0b110), 30, row);

          //  set_row((uint8_t)(0b110), 30, row);
          //  set_row((uint8_t)(0b110), 8, row);
           // set_row((uint8_t)(0b110), 6, row);
          //  set_col((uint8_t)(0b100), 4, col);
           // set_col((uint8_t)(0b100), 5, col);

            //clock these changes into the current pixel.
            GPIOA->BSRR |= 1<<8;
            GPIOA->BRR |= 1<<8;
        }

        /* to finish the transmission
         * Output enable turn high,
         * LATCH is pulsed high then low
         * new address is loaded in PC0-PC3.
         * Output enable is turned off
         */
        GPIOA->BSRR |= 1<<9;
        GPIOB->BSRR |= 1<<4;
        GPIOB->BRR |= 1<<4;
        GPIOC->BRR |= 0xf;
        GPIOC->BSRR |= row;
        GPIOA->BRR |= 1<<9;
    }
    }
}
