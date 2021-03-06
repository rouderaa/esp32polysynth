/*!
 *  @file       PolySynth.h
 *  Project     ESP32-A1S Polyphonic MIDI Synthesizer 
 *  @brief      MIDI controlled Polyphonic ESP32-A1S based synthesizer.
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

#pragma once

#include "WaveGenerator.h"
#include "WaveFactory.h"
#include "AC101.h"

#include "constants.h"

// -----------------------------------------------------------------------------


/*! \brief The polyphonic ESP32-A1S synthesizer.
 */
class PolySynth
{
public:

    void begin();
    void loop();

    void testGenerate(byte pitch1, byte pitch2);

    // MIDI message handling
    void startNote(byte pitch, byte velocity);
    void stopNote(byte pitch, byte velocity);
    void setVolume(uint8_t volume);
    void setStyle(byte style);

    static const byte SINUSSTYLE    = 0;
    static const byte TRIANGLESTYLE = 1;
    static const byte SQUARESTYLE   = 2;
private:
    uint32_t buffer[BUFFERSIZE];
    WaveGenerator wavegenerators[NROFWAVEGENERATORS];
    int bytesWritten; // For debugging
    WaveFactory waveFactory;

    AC101 ac; // Audio chip
    uint8_t volume = 32;
    WaveGenerator *toFreeWaveGenerators;
//    byte style = SINUSSTYLE;
    byte style = TRIANGLESTYLE;

    void initFreeWaveGenerators();
    bool setPinout(int bclk, int wclk, int dout);
    void installDriver(int i2sBufferSize, int i2sNrOfBuffers);
};

// -----------------------------------------------------------------------------
