#include <cmath>
#include <iostream>
#include <cstdint>
#include "oscillator.h"
#include "portaudio.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 2048

struct TestData
{
  uint64_t frameIndex;
  uint64_t maxFrameIndex;
  float* samples;
};

static int playCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
  TestData* data = static_cast<TestData*>(userData);
  float* rptr = &data->samples[data->frameIndex];
  float* wptr = (float*)outputBuffer;
  unsigned int i;
  int finished;
  unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

  (void) inputBuffer;
  (void) timeInfo;
  (void) statusFlags;
  (void) userData;

  if (framesLeft < framesPerBuffer)
  {
    /* final buffer... */
    for (i = 0; i < framesLeft; i++)
    {
      *wptr++ = *rptr++;
    }
    for (; i < framesPerBuffer; i++)
    {
      *wptr++ = 0;
    }
    data->frameIndex += framesLeft;
    finished = paComplete;
  }

  else
  {
    for (i = 0; i < framesPerBuffer; i++)
    {
      *wptr++ = *rptr++;
    }
    data->frameIndex += framesPerBuffer;
    finished = paContinue;
  }
  return finished;
}

void sine_sweep(uint32_t seconds)
{
  std::cout << "Sine sweep" << std::endl;

  TestData data;
  uint32_t numSamples;

  numSamples = seconds * SAMPLE_RATE;
  data.frameIndex = 0;
  data.maxFrameIndex = numSamples;
  data.samples = new float[numSamples];

  Wavetable sinetable;
  sinetable.length = 256;
  sinetable.samples = new float[sinetable.length];

  for (uint32_t i = 0; i < sinetable.length; i++)
  {
    sinetable.samples[i] = (float) sin(((double)i/(double)sinetable.length) * M_PI * 2.0);
  }

  Oscillator osc;
  osc.addWavetable(sinetable);

  double freqVal = 40.0 / SAMPLE_RATE;
  double freqMult = 1.0 + (log(20000.0 / SAMPLE_RATE) - log(freqVal)) / numSamples;

  for (uint32_t i = 0; i < numSamples; i++)
  {
    osc.setFrequency(freqVal);
    data.samples[i] = osc.getSample();
    osc.update();
    freqVal *= freqMult;
  }
  
  PaStreamParameters outputParameters;
  PaStream* stream;
  PaError err = paNoError;

  err = Pa_Initialize();
  outputParameters.device = Pa_GetDefaultOutputDevice();
  if (outputParameters.device == paNoDevice)
  {
    std::cout << "Error: No default output device!" << std::endl;
    Pa_Terminate();
    return;
  }
  
  outputParameters.channelCount = 1;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = 
    Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&stream, NULL, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER,
      paClipOff, playCallback, &data);

  if (err != paNoError)
  {
    std::cout << "Error: Couldn't open stream" << std::endl;
    Pa_Terminate();
    return;
  }
  
  if (stream)
  {
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
      std::cout << "Error: Couldn't start stream" << std::endl;
      Pa_Terminate();
      return;
    }

    while ((err = Pa_IsStreamActive(stream)) == 1)
      Pa_Sleep(100);

    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
      std::cout << "Error: Couldn't close stream" << std::endl;
      Pa_Terminate();
      return;
    }

    std::cout << "Done." << std::endl;
  }

  Pa_Terminate();
}

int main()
{
  sine_sweep(5);
  return 0;
}

