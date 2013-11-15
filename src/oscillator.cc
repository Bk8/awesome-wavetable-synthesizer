#include "oscillator.h"
#include <string.h>

Oscillator::Oscillator() : 
  _phaseAccumulator(0.0),
  _phaseIncrement(0.0),
  _numWavetables(0)
{
}

Oscillator::~Oscillator()
{
}

void Oscillator::addWavetable(const Wavetable& wavetable)
{
  memcpy(&_waveTables+_numWavetables, &wavetable, sizeof (Wavetable));
  memcpy(_waveTables[_numWavetables].samples, &wavetable.samples, wavetable.length);
  _numWavetables++;
}

void Oscillator::setFrequency(double frequency)
{
  _phaseIncrement = frequency; // TODO
}

void Oscillator::update()
{
  _phaseAccumulator += _phaseIncrement;

  if (_phaseAccumulator >= 1.0)
    _phaseAccumulator -= 1.0;
}

float Oscillator::getSample()
{
  Wavetable& wavetable = _waveTables[0];
  double tmp;
  
  tmp = _phaseAccumulator * wavetable.length;
  
  uint32_t intPart = tmp;
  double fraqPart = tmp - intPart;

  float s0 = wavetable.samples[intPart];
  if (++intPart >= wavetable.length)
    intPart = 0;
  float s1 = wavetable.samples[intPart];

  return s0 + (s1 - s0) * fraqPart;
}

