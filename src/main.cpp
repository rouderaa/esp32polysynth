#include <Arduino.h>
#include <MIDI.h>
#include "PolySynth.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

static PolySynth polysynth;

// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html
void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    polysynth.startNote(pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    polysynth.stopNote(pitch, velocity);
}

void handleProgramChange(byte channel, byte number)
{
    Serial.printf("PrChg c:%d n:%d\n\r", channel, number);
    if (number == 0) {
      // Use sinus wave sounds (piano)
      polysynth.setStyle(PolySynth::SINUSSTYLE);
    } else
    if (number == 18) {
      // Use triangle wave sounds (organ)
      polysynth.setStyle(PolySynth::TRIANGLESTYLE);
    } else
    if (number == 36) {
      // Use square wave sounds (computer)
      polysynth.setStyle(PolySynth::SQUARESTYLE);
    } 
}

void setup() {  
  // Serial is for logging
  Serial.begin(115200);
  Serial.printf("ESP32PolySynth version : %s\n\r", ESP32POLYSYNTHVERSION);

  // Attach handlers for MIDI messages coming in through Serial2
  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleProgramChange(handleProgramChange);

  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Serial2 is the MIDI port, MUST be opened AFTER the MIDI.begin)()
  uint32_t config = 134217756U;
  // Rx=IO21, TX=IO19 pins
  Serial2.begin(31250, config, GPIO_NUM_21, GPIO_NUM_19, false);

  polysynth.begin();
  polysynth.setVolume(40);

  // polysynth.testGenerate(69, 69+3); // Debug A4 note, 440 hz and other note
}

void loop() {
  // Check if something is on the MIDI input port
  MIDI.read();
  // Generate tones (waves)
  polysynth.loop();
}
 