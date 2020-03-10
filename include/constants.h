/*!
 *  @file       constants.h
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

#define ESP32POLYSYNTHVERSION "V1.0 2020-03-10"
static const uint32_t WAVEPARTERRORMAX = (100000/2);
static const int NROFSTYLES = 3;
static const int NROFWAVEGENERATORS = 16;
static const int BUFFERSIZE=256; // measured in samples
static const int NROFBUFFERS=2;
static const int APLL_DISABLE = 0;
static const int SAMPLERATE = 192000;
static const int PORTNR = 0;
static const int IIC_CLK=32;
static const int IIC_DATA=33;
static const int IIS_SCLK=27;
static const int IIS_LCLK=26;
static const int IIS_DSIN=25;

static const int TOP=(((0xffff/2)/NROFWAVEGENERATORS)-0x10);
// static const int BASE=((0xffff/2)/NROFWAVEGENERATORS);
static const int BASE=(0);
static const int STEREOTOP=((TOP << 16) + TOP);
static const int STEREOBASE=((BASE << 16) + BASE);

// -----------------------------------------------------------------------------
