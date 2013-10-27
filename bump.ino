#include <Arduino.h>
#include <MIDI.h>

#include <MsTimer2.h>

const int BEATS = 16;

const int LED_PIN = 13;
const int BUF_LEN = 64;

char beat1[BUF_LEN];
char beat2[BUF_LEN];
char beat3[BUF_LEN];

void setup() {
    pinMode(LED_PIN, OUTPUT);
    MIDI.begin(0);
  
    randomSeed(analogRead(0));
    
    fill_beats((char *)&beat1, (int)random(BEATS), BEATS);
    fill_beats((char *)&beat2, (int)random(BEATS), BEATS);
    fill_beats((char *)&beat3, (int)random(BEATS), BEATS);

    set_bpm(100*4);
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

volatile unsigned char do_tick = 0;
void tick() {
    do_tick = 1;
}

void start_playing(int note) {
    MIDI.sendNoteOn(note, 0, 1);
    MIDI.sendNoteOn(note, 127, 1);
}

unsigned char pos = 0;
void on_tick() {
    MIDI.sendNoteOn(36, 0, 1);
    MIDI.sendNoteOn(38, 0, 1);
    MIDI.sendNoteOn(51, 0, 1);
    
    if (beat1[pos] == 'x') {
        MIDI.sendNoteOn(36, 127, 1);
    }
    if (beat2[pos] == 'x') {
        MIDI.sendNoteOn(38, 127, 1);
    }
    if (beat3[pos] == 'x') {
        MIDI.sendNoteOn(51, 127, 1);
    }

    if (++pos == BEATS) {
        pos = 0;
    }
}

void loop() {
    if (do_tick) {
        do_tick = 0;
        on_tick();
    }
}
