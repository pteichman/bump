#include <Arduino.h>
#include <MIDI.h>

#include <FlexiTimer2.h>

const int STEPS = 16;

const int LED_PIN = 13;
const int BUF_LEN = 64;

char beat1[BUF_LEN];
char beat2[BUF_LEN];
char beat3[BUF_LEN];

void setup() {
    pinMode(LED_PIN, OUTPUT);
    MIDI.begin(0);

    randomSeed(analogRead(0));

    fill_beats((char *)&beat1, (int)random(STEPS), STEPS);
    fill_beats((char *)&beat2, (int)random(STEPS), STEPS);
    fill_beats((char *)&beat3, (int)random(STEPS), STEPS);

    // Set up a timer running at ~120bpm. on_tick() is designed
    // to also be used with a MIDI clock, where it gets 24 ticks
    // per quarter note. So this timer needs to tick every
    // 60000 / 24 / 120 = 20.833ms.
    //
    // With float truncation, what the below actually gives us is a
    // tick every 20ms - 115bpm. We'll need to average out the error
    // for more accuracy.
    FlexiTimer2::stop();
    FlexiTimer2::set((int)(60000. / 24. / 120.), &on_tick);
    FlexiTimer2::start();

    MIDI.setHandleClock(&on_first_midi_tick);
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

void on_first_midi_tick() {
    on_tick();

    // disable the automatic timer and switch over to the MIDI clock
    MIDI.setHandleClock(&on_tick);
    FlexiTimer2::stop();
}

volatile unsigned char clock = 0;
void on_tick() {
    if (clock++ > 4) {
        on_beat();
        clock = 0;
    }
}

unsigned char pos = 0;
void on_beat() {
    MIDI.sendNoteOn(36, 0, 1);
    MIDI.sendNoteOn(38, 0, 1);
    MIDI.sendNoteOn(51, 0, 1);
    digitalWrite(LED_PIN, LOW);

    if (beat1[pos] == 'x') {
        MIDI.sendNoteOn(36, 100, 1);
        digitalWrite(LED_PIN, HIGH);
    }

    if (beat2[pos] == 'x') {
        MIDI.sendNoteOn(38, 100, 1);
    }

    if (beat3[pos] == 'x') {
        MIDI.sendNoteOn(51, 100, 1);
    }

    if (++pos == STEPS) {
        pos = 0;
    }
}

void loop() {
    MIDI.read();
}
