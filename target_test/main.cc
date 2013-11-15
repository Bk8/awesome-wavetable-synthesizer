#include "Arduino.h"
#include "Adafruit_MCP4725.h"
#include "oscillator.h"
#include "math.h"

Adafruit_MCP4725 dac;

Oscillator* osc = new Oscillator;
Wavetable sinetable;
double freqVal = 40.0 / 44100;
double freqMult = 1.0 + (log(20000.0 / 44100) - log(freqVal)) / 5*44100;

void setup()
{
  dac.begin(0x62);

  sinetable.length = 256;
  sinetable.samples = new float[sinetable.length];

  for (uint32_t i = 0; i < sinetable.length; i++)
  {
    sinetable.samples[i] = (float) sin(((double)i/(double)sinetable.length) * M_PI * 2.0);
  }

  osc->addWavetable(sinetable);
}


void loop()
{
  osc->setFrequency(880/44100);
  float out = osc->getSample();
  osc->update();
  dac.setVoltage((float)4096 * out, false);
}

