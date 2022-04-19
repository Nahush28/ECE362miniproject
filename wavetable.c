#include <math.h>
#include "midiplay.h"

short int sine[N];
short int sawtooth[N];
short int square[N];
short int hybrid[N];
short int hybrid2[N];

// sine wave
void init_wavetable_sine(void) {
    int x;
    for(x=0; x<N; x++) {
        sine[x] = 32767 * sin(2 * M_PI * x / N);
    }
}

// sawtooth wave
void init_wavetable_sawtooth(void) {
    int x;
    for(x=0; x<N; x++)
        sawtooth[x] = 32767.0 * (x - N/2) / (1.0*N);
}

// square wave
void init_wavetable_square(void) {
    int x;
    for(x=0; x<N; x++)
        if (x < N/2)
            square[x] = -32767;
        else
            square[x] = 32767;
}

// 1/2 amplitude sine wave added to 1/2 amplitude sawtooth wave
void init_wavetable_hybrid(void) {
    int x;
    for(x=0; x<N; x++)
        hybrid[x] = 16383 * sin(2 * M_PI * x / N) + 16383.0 * (x - N/2) / (1.0*N);
}

// 3/4 amplitude sine wave added to 1/4 amplitude sawtooth wave
void init_wavetable_hybrid2(void) {
    int x;
    for(x=0; x<N; x++)
        hybrid2[x] = 3*8191 * sin(2 * M_PI * x / N) + 8191.0 * (x - N/2) / (1.0*N);
}

