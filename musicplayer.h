#ifndef MINIPROJECT_MUSICPLAYER_H
#define MINIPROJECT_MUSICPLAYER_H
#include "midi.h"
#include "midiplay.h"

void init_musicplayer_peripherals();

void init_wavetable_all();

void init_tim2(int n);
void pitch_wheel_change(int time, int chan, int value);
void set_tempo(int time, int value, const MIDI_Header *hdr);
void note_on(int time, int chan, int key, int velo);
void note_off(int time, int chan, int key, int velo);
void init_tim6(void);
void init_dac(void);
void program_change(int time, int chan, int prog);


#endif //MINIPROJECT_MUSICPLAYER_H
