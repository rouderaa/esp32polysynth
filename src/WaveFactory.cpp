/*!
 *  @file       WaveFactory.cpp
 *  Project     ESP32-A1S Polyphonic MIDI Synthesizer 
  * @brief      MIDI controlled Polyphonic ESP32-A1S based synthesizer.
 *  @author     Rob van der Ouderaa
 *  @date       27/02/2020
 *  @license    MIT - Copyright (c) 2020 Rob van der Ouderaa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Arduino.h>
#include <math.h>
#include "PolySynth.h"
#include "Note.h"
#include "WaveFactory.h"

// -----------------------------------------------------------------------------
static char *noteNames[NROFNOTESINOCTAVE] = {
  "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"
};

// Generate sinus wave sample for one note, only the first quarter of the wave
int WaveFactory::makeSinusNote(
    Note *toNote, double frequency, int midiNoteNr, 
    char *toNoteName, int octaveNr
) {
    int bufferSize = (SAMPLERATE/(frequency*4)); // number of samples for quarter of wave
    uint32_t *buffer = (uint32_t *) malloc(bufferSize*4); // 4 bytes per sample
    if (buffer == NULL) {
      Serial.printf("ERROR: could not allocate memory.\n\r");
      while(1); // halt
    }
    double delta = (PI/2)/(double) bufferSize;
    double angle = 0;

    // We want to generate 12 bit samples
    // since we have a dynamic range of 16 bits and we want
    // 16 simultanious waves to be added without possible clipping.
    for(int  index = 0; index < bufferSize; index++) {
      double s = sin(angle)*TOP;
      uint32_t monoSample = s;
      uint32_t v = (monoSample << 16) | monoSample;
      buffer[index] = v;

      angle +=  delta;
    }

    toNote->samples[0] = buffer;
    toNote->sampleSizes[0] = bufferSize;

    double nrOfSamples = (SAMPLERATE/frequency);
    double error = nrOfSamples - trunc(nrOfSamples);
    // wave in 4 parts, per HALF wave sample error
    // since we will compensate per HALF wave
    uint32_t sampleError = (error*WAVEPARTERRORMAX); 
    toNote->sampleErrors[0] = sampleError;

    return(bufferSize);
}

// Generate triangle wave sample for one note, only the first quarter of the wave
int WaveFactory::makeTriangleNote(
    Note *toNote, double frequency, int midiNoteNr, 
    char *toNoteName, int octaveNr
) {
    int bufferSize = (SAMPLERATE/(frequency*4)); // number of samples for quarter of wave
    uint32_t *buffer = (uint32_t *) malloc(bufferSize*4); // 4 bytes per sample
  
    double upDelta = 1.0/(double) bufferSize; // Lineair progression from 0.0 to 1.0
    double upValue = 0.0;

    // We want to generate 12 bit samples
    // since we have a dynamic range of 16 bits and we want
    // 16 simultanious waves to be added without possible clipping.
    for(int  index = 0; index < bufferSize; index++) {
      uint32_t monoSample = (upValue*TOP);
      uint32_t v = (monoSample << 16) | monoSample;
      buffer[index] = v;
      upValue = upValue + upDelta;
    }

    toNote->samples[1] = buffer;
    toNote->sampleSizes[1] = bufferSize;

    double nrOfSamples = (SAMPLERATE/frequency);
    double error = nrOfSamples - trunc(nrOfSamples);
    // wave in 4 parts, per HALF wave sample error
    // since we will compensate per HALF wave
    uint32_t sampleError = (error*WAVEPARTERRORMAX); 
    toNote->sampleErrors[1] = sampleError;

    return(bufferSize);
}

int WaveFactory::makeSquareNote(
    Note *toNote, double frequency, int midiNoteNr, 
    char *toNoteName, int octaveNr
) {
    int bufferSize = (SAMPLERATE/(frequency*4)); // number of samples for quarter of wave
    uint32_t *buffer = (uint32_t *) malloc(bufferSize*4); // 4 bytes per sample
  
    double upValue = 0.9;

    // We want to generate 12 bit samples
    // since we have a dynamic range of 16 bits and we want
    // 16 simultanious waves to be added without possible clipping.
    for(int  index = 0; index < bufferSize; index++) {
      uint32_t monoSample = (upValue*TOP);
      uint32_t v = (monoSample << 16) | monoSample;
      buffer[index] = v;
    }

    toNote->samples[2] = buffer;
    toNote->sampleSizes[2] = bufferSize;

    double nrOfSamples = (SAMPLERATE/frequency);
    double error = nrOfSamples - trunc(nrOfSamples);
    // wave in 4 parts, per HALF wave sample error
    // since we will compensate per HALF wave
    uint32_t sampleError = (error*WAVEPARTERRORMAX); 
    toNote->sampleErrors[2] = sampleError;

    return(bufferSize);
}

void WaveFactory::begin() {
  static const double base = 1.059463094; // 2 to power (1/12)
  static const double tuningbase = 27.50; // A0=27.5hz

  // Fill first octave starting at the A0 note
  unsigned long totalNrOfSamples = 0L;
  int octaveNr = 0;
  int noteCount = 9; // Start at A0 note for midi pitch nr 21
  for(int index = MINMIDINOTES; index < (MINMIDINOTES+NROFNOTESINOCTAVE); index++) {
    double frequency = tuningbase*pow(base, index-MINMIDINOTES);
    // Serial.printf("Note: %d Frequency:%f\n\r", index, frequency);
    char *toNoteName = noteNames[(index-MINMIDINOTES) % 12];
    // Make samples for the several types for each note
    totalNrOfSamples += makeSinusNote(&notes[index], frequency, index, toNoteName, octaveNr);
    totalNrOfSamples += makeTriangleNote(&notes[index], frequency, index, toNoteName, octaveNr);
    totalNrOfSamples += makeSquareNote(&notes[index], frequency, index, toNoteName, octaveNr);

    Note *toNote = &notes[index];
    toNote->delta = 1;
    toNote->midiNoteNr = index;
    toNote->frequency = frequency;
    sprintf(toNote->name, "%s%d", toNoteName, octaveNr);

    noteCount++;
    if (noteCount == NROFNOTESINOCTAVE) {
      noteCount = 0;
      octaveNr++;
    }
  }
  // Serial.printf("TotalNrOfSamples: %lu (%lu bytes)\n\r", totalNrOfSamples, totalNrOfSamples*4L);

  // Fill next octaves using the 0 and 1 octave note samples and bigger deltas
  int multiplier = 2; // double this for each octave
  int baseCount = 0;
  Note *toBaseNote = &notes[MINMIDINOTES];
  for(int index = (MINMIDINOTES+NROFNOTESINOCTAVE); index <= MAXMIDINOTES; index++) {
    double frequency = tuningbase*pow(base, index-MINMIDINOTES);
    // Serial.printf("Note: %d Frequency:%f\n\r", index, frequency);

    Note *toNote = &notes[index];
    toNote->midiNoteNr = index;
    for(int styleIndex = 0; styleIndex < NROFSTYLES; styleIndex++) {
      toNote->samples[styleIndex] = toBaseNote->samples[styleIndex];
      toNote->sampleSizes[styleIndex] = toBaseNote->sampleSizes[styleIndex]; 
    }
    toNote->delta = multiplier;
    toNote->frequency = frequency;
    sprintf(toNote->name, "%s%d", noteNames[toBaseNote->midiNoteNr-MINMIDINOTES], octaveNr);

    toBaseNote++; // Next basenote of lowest octave
    noteCount++;
    if (noteCount == NROFNOTESINOCTAVE) {
      noteCount = 0;
      octaveNr++;
    }
    baseCount++;
    if (baseCount == NROFNOTESINOCTAVE) {
      baseCount = 0;
      toBaseNote = &notes[MINMIDINOTES];
      multiplier = multiplier*2;
    }
  }
  
}

Note *WaveFactory::getNote(int noteNr) {
  if ((noteNr >= MINMIDINOTES) && (noteNr <= MAXMIDINOTES))
    return &notes[noteNr];

  return NULL;
}