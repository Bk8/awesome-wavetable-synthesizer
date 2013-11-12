#include "oscillator.h"
#include <iostream>

Oscillator::Oscillator() : 
  _phaseAccumulator(0.0),
  _phaseIncrement(0.0)
{
}

Oscillator::~Oscillator()
{
}

void Oscillator::addWavetable(const Wavetable& wavetable)
{
  _waveTables.push_back(wavetable);
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

