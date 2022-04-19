#include "stm32f0xx.h"
#include <stdint.h>
#include "midi.h"
#include "midiplay.h"

// The number of simultaneous voices to support.
#define VOICES 15

// An array of "voices".  Each voice can be used to play a different note.
// Each voice can be associated with a channel (explained later).
// Each voice has a step size and an offset into the wave table.
struct {
    uint8_t in_use;
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
    int     last_sample;
    int16_t *wavetable;
} voice[VOICES];

// We'll use the Timer 6 IRQ to recompute samples and feed those
// samples into the DAC.
void TIM6_DAC_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here.
    TIM6->SR &= ~TIM_SR_UIF;

    int sample = 0;
    for(int x=0; x < sizeof voice / sizeof voice[0]; x++) {
        if (voice[x].in_use) {
            voice[x].offset += voice[x].step;
            if (voice[x].offset >= N<<16)
                voice[x].offset -= N<<16;
            voice[x].last_sample = voice[x].wavetable[voice[x].offset>>16];
            sample += (voice[x].last_sample * voice[x].volume) >> 4;
        }
        else if (voice[x].volume != 0) {
            sample += (voice[x].last_sample * voice[x].volume) >> 4;
            voice[x].volume --;
        }
    }
    sample = (sample >> 10) + 2048;
    if (sample > 4095)
        sample = 4095;
    else if (sample < 0)
        sample = 0;
    DAC->DHR12R1 = sample;
}

int16_t *channel_wavetable[20]; // It's zero, by default.

void program_change(int time, int chan, int prog)
{
    channel_wavetable[chan] = hybrid; // Use this by default.
    if (prog >= 71 && prog <= 79) // flutes, recorders, etc.
        channel_wavetable[chan] = sine;
    if (prog == 29) // overdriven guitar
        channel_wavetable[chan] = square;
    if (prog >= 52 && prog <= 54) // choir voices
        channel_wavetable[chan] = sine;
}

// Initialize the DAC so that it can output analog samples
// on PA4.  Configure it to be triggered by TIM6 TRGO.
void init_dac(void)
{
    // TODO: you fill this in.
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;

}

// Initialize Timer 6 so that it calls TIM6_DAC_IRQHandler
// at exactly RATE times per second.  You'll need to select
// a PSC value and then do some math on the system clock rate
// to determine the value to set for ARR.  Set it to trigger
// the DAC by enabling the Update Trigger in the CR2 MMS field.
void init_tim6(void)
{
    // TODO: you fill this in.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 480-1;
    TIM6->ARR = 100000/RATE-1;
    TIM6->DIER = TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM6->CR2 |= 0x20;
    NVIC->ISER[0] = 1<<TIM6_DAC_IRQn;

    NVIC_SetPriority(TIM6_DAC_IRQn, 0);


}

// Find the voice current playing a note, and turn it off.
void note_off(int time, int chan, int key, int velo)
{
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].note == key) {
            voice[n].in_use = 0; // disable it first...
            voice[n].chan = 0;   // ...then clear its values
            voice[n].note = key;
            voice[n].step = step[key];
            return;
        }
    }
}

// Find an unused voice, and use it to play a note.
void note_on(int time, int chan, int key, int velo)
{
    if (velo == 0) {
        note_off(time, chan, key, velo);
        return;
    }
    int n;

    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (channel_wavetable[chan] == 0)
            voice[n].wavetable = hybrid;
        else
            voice[n].wavetable = channel_wavetable[chan];
        if (voice[n].in_use == 0) {
            voice[n].note = key;
            voice[n].step = step[key];
            voice[n].offset = 0;
            voice[n].volume = velo;
            voice[n].chan = chan;
            voice[n].in_use = 1;
            return;
        }
    }
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI file header.
    TIM2->ARR = value/hdr->divisions - 1;
}

const float pitch_array[] = {
0.943874, 0.945580, 0.947288, 0.948999, 0.950714, 0.952432, 0.954152, 0.955876,
0.957603, 0.959333, 0.961067, 0.962803, 0.964542, 0.966285, 0.968031, 0.969780,
0.971532, 0.973287, 0.975046, 0.976807, 0.978572, 0.980340, 0.982111, 0.983886,
0.985663, 0.987444, 0.989228, 0.991015, 0.992806, 0.994599, 0.996396, 0.998197,
1.000000, 1.001807, 1.003617, 1.005430, 1.007246, 1.009066, 1.010889, 1.012716,
1.014545, 1.016378, 1.018215, 1.020054, 1.021897, 1.023743, 1.025593, 1.027446,
1.029302, 1.031162, 1.033025, 1.034891, 1.036761, 1.038634, 1.040511, 1.042390,
1.044274, 1.046160, 1.048051, 1.049944, 1.051841, 1.053741, 1.055645, 1.057552,
};

void pitch_wheel_change(int time, int chan, int value)
{
    //float multiplier = pow(STEP1, (value - 8192.0) / 8192.0);
    float multiplier = pitch_array[value >> 8];
    for(int n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].chan == chan) {
            voice[n].step = step[voice[n].note] * multiplier;
        }
    }
}

// An array of structures that say when to turn notes on or off.
struct {
    int when;
    uint8_t note;
    uint8_t volume;
} events[] = {
        {480,84,0x73}, {556,84,0x00}, {960,84,0x74}, {1008,84,0x00},
        {1440,91,0x76}, {1520,91,0x00}, {1920,91,0x79}, {1996,91,0x00},
        {2400,93,0x76}, {2472,93,0x00}, {2640,94,0x67}, {2720,94,0x00},
        {2880,96,0x67}, {2960,96,0x00}, {3120,93,0x6d}, {3180,93,0x00},
        {3360,91,0x79}, {3440,91,0x00}, {4320,89,0x70}, {4408,89,0x00},
        {4800,89,0x73}, {4884,89,0x00}, {5280,88,0x73}, {5360,88,0x00},
        {5760,91,0x79}, {5836,91,0x00}, {6240,86,0x79}, {6308,86,0x00},
        {6720,86,0x76}, {6768,86,0x00}, {7200,84,0x76}, {7252,84,0x00},
        {8160,84,0x73}, {8236,84,0x00}, {8640,84,0x74}, {8688,84,0x00},
        {9120,91,0x76}, {9200,91,0x00}, {9600,91,0x79}, {9676,91,0x00},
        {10080,93,0x76}, {10152,93,0x00}, {10320,94,0x67}, {10400,94,0x00},
        {10560,96,0x67}, {10640,96,0x00}, {10800,93,0x6d}, {10860,93,0x00},
        {11040,91,0x79}, {11120,91,0x00}, {12000,86,0x76}, {12080,86,0x00},
        {12480,86,0x73}, {12552,86,0x00}, {13440,84,0x6d}, {13440,88,0x73},
        {13508,88,0x00}, {13512,84,0x00}, {13920,86,0x76}, {14004,86,0x00},
        {14400,86,0x76}, {14472,86,0x00}, {15152,81,0x3b}, {15184,83,0x44},
        {15188,81,0x00}, {15220,84,0x46}, {15228,83,0x00}, {15248,86,0x57},
        {15264,84,0x00}, {15284,88,0x5c}, {15292,86,0x00}, {15308,89,0x68},
        {15320,88,0x00}, {15336,91,0x6d}, {15344,89,0x00}, {15364,93,0x6d},
        {15368,91,0x00}, {15460,93,0x00},
};

//#if 0
int time = 0;
int n = 0;

uint8_t notes[] = { 60,62,64,65,67,69,71,72,71,69,67,65,64,62,60,0 };
uint8_t num = sizeof notes / sizeof notes[0] - 1;

// Configure timer 2 so that it invokes the Update interrupt
// every n microseconds.  To do so, set the prescaler to divide
// by 48.  Then the CNT will count microseconds up to the ARR value.
// Basically ARR = n-1
// Set the ARPE bit in the CR1 so that the timer waits until the next
// update before changing the effective ARR value.
// Call NVIC_SetPriority() to set a low priority for Timer 2 interrupt.
// See the lab 6 text to understand how to do so.
void init_tim2(int n) {
    // TODO: you fill this in.
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 48-1;
    TIM2->ARR = n-1;
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<TIM2_IRQn;

    TIM2->CR1 |= TIM_CR1_ARPE;

    NVIC_SetPriority(TIM2_IRQn, 3);

}

void TIM2_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here!
    TIM2->SR &= ~TIM_SR_UIF;

    midi_play();
}
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

int main(void)
{
    /**
     * This block of code is for the initialization of the MIDI file player. To
     * use simply un-comment and exclude all but one .c MIDI file
     */

    init_wavetable_sine();
    init_wavetable_square();
    init_wavetable_sawtooth();
    init_wavetable_hybrid();
    init_wavetable_hybrid2();

    init_dac();
    init_tim6();


    MIDI_Player *mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_tim2(10417);
    for(;;) {
        asm("wfi");
        // If we hit the end of the MIDI file, start over.
        if (mp->nexttick == MAXTICKS)
            mp = midi_init(midifile);

    }

    //// The following code is for running the 32x64 LED Matrix.

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
    for(int i = 0; i< 16; i++){ // 32 rows but every for every row select you can drive 2 rows.
        for(int j = 0; j < 64; j++){ // need to clock 64 times for each column on the display
            if(j != 32) // simple if condition which says turn the upper current pixel green if its not the middle most one
                GPIOB->BSRR |= 1<<12;
            else
                GPIOB->BRR |= 1<<12;

            if(i == 15) // another simple if condition which says turn the bottom current pixel red if its is the last row
                GPIOB->BSRR |= 1<<9;
            else
                GPIOB->BRR |= 1<<9;

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
        GPIOC->BSRR |= i;
        GPIOA->BRR |= 1<<9;
    }
    }
}
