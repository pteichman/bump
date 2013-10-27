#include <Arduino.h>
#include <MIDI.h>

#include <MsTimer2.h>

const int LED_PIN = 13;
const int BUF_LEN = 64;

char beat1[BUF_LEN];
char beat2[BUF_LEN];

void setup() {
    pinMode(LED_PIN, OUTPUT);
    MIDI.begin(0);
  
    fill_beats((char *)&beat1, 4, 16);
    fill_beats((char *)&beat2, 6, 16);

    set_bpm(460);
}

void set_bpm(float bpm) {
    if (bpm <= 0) {
        bpm = 1;
    }
  
    int ms = (int)(60000. / bpm);
  
    MsTimer2::stop();
    MsTimer2::set(ms, &tick);
    MsTimer2::start();
}

void fill_beats(char *beat, int num_beats, int num_steps) {
    int cur = num_steps;
    for (int i=0; i<num_steps; i++) {
        if (cur >= num_steps) {
            cur -= num_steps;
            *beat = 'x';
        } else {
            *beat = '.';
        }
        beat++;
        cur += num_beats;
    }
}

volatile unsigned int ticks = 0;
void tick() {
    ticks++;
}

void start_playing(int note) {
    MIDI.sendNoteOn(note, 0, 1);
    MIDI.sendNoteOn(note, 127, 1);
}

void on_tick() {
    if (beat1[ticks%16] == 'x') start_playing(36);
    if (beat2[ticks%16] == 'x') start_playing(38);
}

unsigned int prev_ticks = -1;
void loop() {
    if (prev_ticks != ticks) {
        on_tick();
    }
    prev_ticks = ticks;
}
