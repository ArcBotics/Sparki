/*******************************************
 Melody
 
This example shows how to make a short melody
with Sparki's buzzer.

This example is based on the Arduino example
by Tom Igoe.
********************************************/
#include <Sparki.h> // include the sparki library
#include "pitches.h" // include a list of pitches

// notes in the melody:
int melody[] = { NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4 };

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

void setup() {
  // play each note in the arrays
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // calculate the note duration as 1 second divided by note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    sparki.beep(melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    sparki.noBeep();
  }
}

void loop() {
  // no need to repeat the melody.
}
