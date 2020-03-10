/*!
 *  @file       PolySynth.cpp
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

#include "PolySynth.h"
#include "WaveGenerator.h"

// -----------------------------------------------------------------------------
bool PolySynth::setPinout(int bclk, int wclk, int dout)
{
  i2s_pin_config_t pins = {
    .bck_io_num = bclk,
    .ws_io_num = wclk,
    .data_out_num = dout,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_set_pin((i2s_port_t)0, &pins);

  return true;
}

void PolySynth::installDriver(int i2sBufferSize, int i2sNrOfBuffers) {
    i2s_mode_t mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_comm_format_t comm_fmt = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
    int dma_buf_count=i2sNrOfBuffers;
    int use_apll=APLL_DISABLE;

      // Setting up I2S configuration
    int buf_len = i2sBufferSize;  // samples of 4 bytes each
    i2s_config_t i2s_config_dac = {
      .mode = mode ,
      .sample_rate = SAMPLERATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = comm_fmt,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // lowest interrupt priority
      .dma_buf_count = dma_buf_count,
      .dma_buf_len = buf_len,
      .use_apll = use_apll // Use audio PLL
    };
    if (i2s_driver_install((i2s_port_t)PORTNR, &i2s_config_dac, 0, NULL) != ESP_OK) {
      Serial.printf("ERROR: Unable to install I2S\n");
    }

    // Assign I2S pins to their function
    setPinout(IIS_SCLK /*bclkPin*/, IIS_LCLK /*wclkPin*/, IIS_DSIN /*doutPin*/);
}

void PolySynth::initFreeWaveGenerators() {
    toFreeWaveGenerators = &wavegenerators[0];

    WaveGenerator *toWaveGenerator;

    // link wave generators together in free linked list
    for(int index = 0; index < (NROFWAVEGENERATORS-1); index++) {
      toWaveGenerator = &wavegenerators[index];
      toWaveGenerator->toNextFreeWaveGenerator = 
        &wavegenerators[index+1];
    }
    // Last wavegenerator has no next
    toWaveGenerator = &wavegenerators[NROFWAVEGENERATORS-1];
    toWaveGenerator->toNextFreeWaveGenerator = NULL;
}

void PolySynth::begin() {
    // Initialise free list of wave generators
    initFreeWaveGenerators();

    // IO22 is debug pin output to channel B of Picoscope
    pinMode(GPIO_NUM_22, OUTPUT);
    digitalWrite(GPIO_NUM_22, HIGH);
    digitalWrite(GPIO_NUM_22, LOW);

    // Setting up I2C control lines to AC101
    while (not ac.begin(IIC_DATA, IIC_CLK))
    {
        Serial.printf("ERROR: AC101 failed\n\r");
        delay(1000);
    }
   
    ac.SetVolumeHeadphone(volume);
    ac.SetVolumeSpeaker(0);  
    
    installDriver(BUFFERSIZE, NROFBUFFERS);

    // Start I2S signal
    i2s_start((i2s_port_t) 0);

    waveFactory.begin(); // Generates waves for the MIDI notes
}

void PolySynth::loop() {

    // measure time used for wave generation
    digitalWrite(GPIO_NUM_22, HIGH);
    
    // Use first generator to fill buffer initially
    WaveGenerator *wg = &wavegenerators[0];
    wg->setSamplesInBuffer(buffer, BUFFERSIZE);
    if (wg->clearStopping()) {
        wg->toNextFreeWaveGenerator = toFreeWaveGenerators;
        toFreeWaveGenerators = wg;
    }

    // Add samples to buffer for each following generator
    for(int index = 1; index < NROFWAVEGENERATORS; index++) {
        WaveGenerator *wg = &wavegenerators[index];
        wg->addSamplesToBuffer(buffer, BUFFERSIZE);
        if (wg->clearStopping()) {
            wg->toNextFreeWaveGenerator = toFreeWaveGenerators;
            toFreeWaveGenerators = wg;
        }
    }

    digitalWrite(GPIO_NUM_22, LOW);

    // write buffer to AC101
    size_t bytesWritten;
    i2s_write((i2s_port_t)PORTNR, buffer, BUFFERSIZE*4, &bytesWritten, portMAX_DELAY);
    if (bytesWritten != (BUFFERSIZE*4)) {
        Serial.printf("ERROR: I2S write could not send bytes\n\r");
    }
}

// Debug function to just run some wavesources with a specific pitch
void PolySynth::testGenerate(byte pitch1, byte pitch2) {
    WaveGenerator *wg1 = &wavegenerators[0];
    Note *toNote = waveFactory.getNote(pitch1);
    wg1->setWave(toNote->samples[0], toNote->sampleSizes[0], toNote->delta, 0);

    WaveGenerator *wg2 = &wavegenerators[1];
    toNote = waveFactory.getNote(pitch2);
    wg2->setWave(toNote->samples[0], toNote->sampleSizes[0], toNote->delta, 0);
}

void PolySynth::setVolume(byte volume) {
    ac.SetVolumeHeadphone(volume);
    ac.SetVolumeSpeaker(volume);
}

void PolySynth::startNote(byte pitch, byte velocity) {
  if (toFreeWaveGenerators != NULL) {
    // Find free wavegenerator
    WaveGenerator *toWaveGenerator = toFreeWaveGenerators;
  
    // Remove wavegenerator from list of free wavegenerators
    toFreeWaveGenerators = toWaveGenerator->toNextFreeWaveGenerator;

    // Set pitch
    Note *toNote = waveFactory.getNote(pitch);
    toWaveGenerator->setWave(toNote->samples[style], toNote->sampleSizes[style], toNote->delta, toNote->sampleErrors[style]);
    // Remember in the note that is playing, which wavegenerator is used
    toNote->toWaveGenerator = toWaveGenerator;

    Serial.printf("+ n:%s p:%d f:%f\n\r", toNote->name, pitch, toNote->frequency);
  }
}

void PolySynth::stopNote(byte pitch, byte velocity) {
  Note *toNote = waveFactory.getNote(pitch);
  WaveGenerator *toWaveGenerator = (WaveGenerator *) toNote->toWaveGenerator;
  if (toWaveGenerator != NULL)
    toWaveGenerator->clearWave();
  toNote->toWaveGenerator = NULL;  

  Serial.printf("- n:%s p:%d\n\r", toNote->name, pitch);
}

void PolySynth::setStyle(byte newStyle) {
  style = newStyle;
}