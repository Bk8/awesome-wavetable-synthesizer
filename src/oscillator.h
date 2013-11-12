#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <vector>
#include <cstdint>

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
    std::vector<Wavetable> _waveTables;
};

#endif /* OSCILLATOR_H */
