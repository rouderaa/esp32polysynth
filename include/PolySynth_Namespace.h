/*!
 *  @file       PolySynth_Namespace.h
 *  Project     ESP32-A1S Polyphonic MIDI Synthesizer
 *  @brief      MIDI controlled Polyphonic ESP32-A1S based synthesizer - Namespace declaration
 *  @author     Rob van der Ouderaa
 *  @date       27/02/2020
 *  @license    MIT - Copyright (c) 2015 Francois Best
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

#define POLYSYNTH_NAMESPACE             polysynth
#define BEGIN_POLYSYNTH_NAMESPACE       namespace POLYSYNTH_NAMESPACE {
#define END_POLYSYNTH_NAMESPACE         }

#define USING_NAMESPACE_POLYSYNTH       using namespace POLYSYNTH_NAMESPACE;

BEGIN_POLYSYNTH_NAMESPACE

END_POLYSYNTH_NAMESPACE
