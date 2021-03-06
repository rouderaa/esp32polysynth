/*!
 *  @file       WaveSource.h
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

#include <stdint.h>
#include "Note.h"

// -----------------------------------------------------------------------------

static const int MINMIDINOTES=21;
static const int MAXMIDINOTES=127;
static const int NROFNOTESINOCTAVE=12;

/*! \brief Class that stores samples for one waveform for each note.
 */
class WaveFactory
{
public:
    void begin();
    // Notenr from 0..255 corresponding to MIDI notes
    Note *getNote(int noteNr);

private:
    Note notes[MAXMIDINOTES+1];

    int makeSinusNote(
        Note *toNote, double frequency, int midiNoteNr, 
        char *toNoteName, int octaveNr
    );
    int makeTriangleNote(
        Note *toNote, double frequency, int midiNoteNr, 
        char *toNoteName, int octaveNr
    );
    int makeSquareNote(
        Note *toNote, double frequency, int midiNoteNr, 
        char *toNoteName, int octaveNr
    );

};

// -----------------------------------------------------------------------------
