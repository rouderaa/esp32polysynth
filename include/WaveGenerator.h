/*!
 *  @file       WaveGenerator.h
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

#include "WaveFactory.h"

// -----------------------------------------------------------------------------

/*! \brief Class that gerates samples for one waveform.
 */
class WaveGenerator
{
public:
    void begin();
    void setWave(uint32_t wave[], int waveSize, int delta, uint32_t error);
    void clearWave();
    void setSamplesInBuffer(uint32_t buffer[], int bufferSize);
    void addSamplesToBuffer(uint32_t buffer[], int bufferSize);
    void printSamples(uint16_t *toSamples, int samplesSize);
    void printBuffer(uint32_t buffer[], int bufferSize);
    bool clearStopping();

    WaveGenerator *toNextFreeWaveGenerator;

private:
    int state = 0;
    bool stopping = false;

    uint32_t *toStartWave;
    uint32_t *toEndWave;
    uint32_t *toWave;
    int delta = 0;
    uint32_t prevValue = 0; // Used for debugging
    uint32_t sampleError = 0; // used to compensate for sample versus frequency misalignment
    uint32_t totalError = 0; // used to compensate for sample versus frequency misalignment
};

// -----------------------------------------------------------------------------
