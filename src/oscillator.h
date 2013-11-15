#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

struct Wavetable
{
  uint32_t length;
  double topFrequency;
  float* samples;
};

class Oscillator
{
  public:
    Oscillator();
    ~Oscillator();

    void addWavetable(const Wavetable&);
    void setFrequency(double);
    void update();
    float getSample();

  private:
    double _phaseAccumulator;
    double _phaseIncrement;
    uint16_t _numWavetables;
    Wavetable _waveTables[128];
};

#endif /* OSCILLATOR_H */
