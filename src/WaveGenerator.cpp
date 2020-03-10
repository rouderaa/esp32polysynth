/*!
 *  @file       WaveSource.cpp
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
#include "driver/i2s.h"

#include "WaveGenerator.h"
#include "WaveFactory.h"

#include "constants.h"

// -----------------------------------------------------------------------------
void WaveGenerator::setWave(uint32_t wave[], int waveSize, int d, uint32_t error) {
  toStartWave = wave;
  toEndWave = wave + (waveSize - 1);
  toWave = wave;
  state = 1;
  delta = d;
  stopping = false;
  sampleError = error; // Calculated error per HALF wave
  totalError = 0;
}

void WaveGenerator::clearWave() {
  stopping = true; // soft stopping, wait until state 4 transit to state 1
}

static inline uint32_t check(uint32_t invalue) {
  uint32_t checkValue = invalue & 0xffff;
  if (checkValue > (0xffff >> 1)) {
    // pseudo negative
    checkValue = 0xffff - checkValue;
    if (checkValue > 0x1000) {
      Serial.printf("ERROR: Sample overflow: %X\n\r", checkValue);
    }
  } else {
    if (checkValue > 0x1000) {
      Serial.printf("ERROR: Sample overflow: %X\n\r", checkValue);
    }
  }

  return invalue;
}

// Returns true if sample generation stopped
void WaveGenerator::setSamplesInBuffer(uint32_t buffer[], int bufferSize) {
  int bufferIndex = 0;
  while(bufferIndex < bufferSize) {
    switch(state) {
      // Idle state, just generate stereo 12 bits mean value all the time
      case 0:
        while(bufferIndex < bufferSize) {
          buffer[bufferIndex] = STEREOBASE;
          bufferIndex++;
        }
        break;
      // In first quarter of wave generation
      case 1:
        // Serial.printf("State:1\n\r");
        while((bufferIndex < bufferSize) && (toWave <= toEndWave)) {
          buffer[bufferIndex] = *toWave;
          bufferIndex++;
          toWave = toWave + delta; // go forwards
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          // End of first quarter reached, continue filling buffer
          state = 2;
          toWave = toEndWave;
          // Find out if we need to add an  additional sample
          totalError = totalError + sampleError;
          if (totalError > WAVEPARTERRORMAX) {
            state = 5;
          }
        }
        break;
      // In second quarter of wave generation
      case 2:
        // Serial.printf("State:2\n\r");
        while((bufferIndex < bufferSize) && (toWave >= toStartWave)) {
          buffer[bufferIndex] = *toWave;
          bufferIndex++;
          toWave = toWave - delta; // go backwards 
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          if (stopping) {
            state = 0; // Goto idle state
          }
          else {
            state = 3; // next wave generation
          }
          toWave = toStartWave;
        }
        break;
      // In third quarter of wave generation
      case 3:
        // Serial.printf("State:3\n\r");
        while((bufferIndex < bufferSize) && (toWave <= toEndWave)) {
          buffer[bufferIndex] = -(*toWave);
          bufferIndex++;
          toWave = toWave + delta;
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          state = 4; // next wave generation
          toWave = toEndWave;
          // Find out if we need to add an  additional sample
          totalError = totalError + sampleError;
          if (totalError > WAVEPARTERRORMAX) {
            state = 6;
          }
        }
        break;
      // In fourth quarter of wave generation
      case 4:
        // Serial.printf("State:4\n\r");
        while((bufferIndex < bufferSize) && (toWave >= toStartWave)) {
          buffer[bufferIndex] = -(*toWave);
          bufferIndex++;
          toWave = toWave - delta; // go backwards 
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          state = 1; // next wave generation
          toWave = toStartWave;
        }
        break;
      case 5:
        if (bufferIndex < bufferSize) {
          // Add a sample to the buffer to compensate for HALF wave error
          totalError = totalError - WAVEPARTERRORMAX;
          buffer[bufferIndex] = *toEndWave;
          bufferIndex++;
          state = 2;
        }
        break;
      case 6:
        if (bufferIndex < bufferSize) {
          // Add a sample to the buffer to compensate for HALF wave error
          totalError = totalError - WAVEPARTERRORMAX;
          buffer[bufferIndex] = -(*toEndWave);
          bufferIndex++;
          state = 4;
        }
        break;
    }
  }
}

void WaveGenerator::addSamplesToBuffer(uint32_t buffer[], int bufferSize) {
  int bufferIndex = 0;
  while(bufferIndex < bufferSize) {
    switch(state) {
      // Idle state, just generate stereo 12 bits mean value all the time
      case 0:
        while(bufferIndex < bufferSize) {
          buffer[bufferIndex] += STEREOBASE;
          bufferIndex++;
        }
        break;
      // In first quarter of wave generation
      case 1:
        // Serial.printf("State:1\n\r");
        while((bufferIndex < bufferSize) && (toWave <= toEndWave)) {
          buffer[bufferIndex] += *toWave;
          bufferIndex++;
          toWave = toWave + delta; // go forwards
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          // End of first quarter reached, continue filling buffer
          state = 2;
          toWave = toEndWave;
          // Find out if we need to add an  additional sample
          totalError = totalError + sampleError;
          if (totalError > WAVEPARTERRORMAX) {
            state = 5;
          }
        }
        break;
      // In second quarter of wave generation
      case 2:
        // Serial.printf("State:2\n\r");
        while((bufferIndex < bufferSize) && (toWave >= toStartWave)) {
          buffer[bufferIndex] += *toWave;
          bufferIndex++;
          toWave = toWave - delta; // go backwards 
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          if (stopping) {
            state = 0; // Goto idle state
          }
          else {
            state = 3; // next wave generation
          }
          toWave = toStartWave;
        }
        break;
      // In third quarter of wave generation
      case 3:
        // Serial.printf("State:3\n\r");
        while((bufferIndex < bufferSize) && (toWave <= toEndWave)) {
          buffer[bufferIndex] += -(*toWave);
          bufferIndex++;
          toWave = toWave + delta;
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          state = 4; // next wave generation
          toWave = toEndWave;
          // Find out if we need to add an  additional sample
          totalError = totalError + sampleError;
          if (totalError > WAVEPARTERRORMAX) {
            state = 6;
          }
        }
        break;
      // In fourth quarter of wave generation
      case 4:
        // Serial.printf("State:4\n\r");
        while((bufferIndex < bufferSize) && (toWave >= toStartWave)) {
          buffer[bufferIndex] += -(*toWave);
          bufferIndex++;
          toWave = toWave - delta; // go backwards 
        }
        // printBuffer(buffer, bufferIndex); // DEBUG
        if (bufferIndex < bufferSize) {
          state = 1; // next wave generation
          toWave = toStartWave;
        }
        break;
      case 5:
        if (bufferIndex < bufferSize) {
          // Add a sample to the buffer to compensate for HALF wave error
          totalError = totalError - WAVEPARTERRORMAX;
          buffer[bufferIndex] += *toEndWave;
          bufferIndex++;
          state = 2;
        }
        break;
      case 6:
        if (bufferIndex < bufferSize) {
          // Add a sample to the buffer to compensate for HALF wave error
          totalError = totalError - WAVEPARTERRORMAX;
          buffer[bufferIndex] += -(*toEndWave);
          bufferIndex++;
          state = 4;
        }
        break;
    }
  }
}

void WaveGenerator::printSamples(uint16_t *toSamples, int samplesSize) {
  for(int index = 0; index < samplesSize; index++) {
    Serial.printf("%X=%X ", index, *toSamples);
    if (((index & 0xfff8) == index) && (index > 0)) {
      Serial.printf("\n\r");
    }
    toSamples++;
  }
  Serial.printf("\n\r");
}

// Note: only displays lower FFFF of value
void WaveGenerator::printBuffer(uint32_t buffer[], int bufferSize) {
  for(int index = 0; index < bufferSize; index++) {
    Serial.printf("%X=%X ", index, (buffer[index] & 0xffff));
    if (((index & 0xfff8) == index) && (index > 0)) {
      Serial.printf("\n\r");
    }
  }
  Serial.printf("\n\r");
}

// Test if we have gone from state 4 to state 0 and want to stop
bool WaveGenerator::clearStopping() {
  if ((stopping) && (state == 0)) {
    stopping = false;
    return true; // signal clear worked
  }
  return false; // no clear signal
}

