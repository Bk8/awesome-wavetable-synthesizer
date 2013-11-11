
#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "oscillator.h"
#include "math.h"

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (2048)
#define NUM_SECONDS     (5)
#define TABLE_SIZE 100
#define NUM_CHANNELS    (1)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/
/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (0)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

typedef struct
{
  int          frameIndex;  /* Index into sample array. */
  int          maxFrameIndex;
  SAMPLE      *recordedSamples;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may be called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */
static int playCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
  paTestData *data = (paTestData*)userData;
  SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
  SAMPLE *wptr = (SAMPLE*)outputBuffer;
  unsigned int i;
  int finished;
  unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

  (void) inputBuffer; /* Prevent unused variable warnings. */
  (void) timeInfo;
  (void) statusFlags;
  (void) userData;

  if( framesLeft < framesPerBuffer )
  {
    /* final buffer... */
    for( i=0; i<framesLeft; i++ )
    {
      *wptr++ = *rptr++;  /* left */
      if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
    }
    for( ; i<framesPerBuffer; i++ )
    {
      *wptr++ = 0;  /* left */
      if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
    }
    data->frameIndex += framesLeft;
    finished = paComplete;
  }
  else
  {
    for( i=0; i<framesPerBuffer; i++ )
    {
      *wptr++ = *rptr++;  /* left */
      if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
    }
    data->frameIndex += framesPerBuffer;
    finished = paContinue;
  }
  return finished;
}

/*******************************************************************/
int main(void);
int main(void)
{
  PaStreamParameters  inputParameters,
                      outputParameters;
  PaStream*           stream;
  PaError             err = paNoError;
  paTestData          data;
  int                 i;
  int                 totalFrames;
  int                 numSamples;
  int                 numBytes;
  SAMPLE              max, val;
  double              average;


  data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
  data.frameIndex = 0;
  numSamples = totalFrames * NUM_CHANNELS;
  numBytes = numSamples * sizeof(SAMPLE);
  data.recordedSamples = (SAMPLE *) malloc( numBytes ); /* From now on, recordedSamples is initialised. */
  if( data.recordedSamples == NULL )
  {
    printf("Could not allocate record array.\n");
  }
  for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

  err = Pa_Initialize();
  /* Playback recorded data.  -------------------------------------------- */
  data.frameIndex = 0;
  Oscillator osc, osc2, osc3;

  printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

  /* initialise sinusoidal wavetable */
  Wavetable wavetable;
  wavetable.length = TABLE_SIZE;
  wavetable.samples = new float[wavetable.length];
  for (int i = 0; i < wavetable.length; i++)
  {
    wavetable.samples[i] = (float) sin( ((double)i/(double)wavetable.length) * M_PI * 2. );
  }
  osc.addWavetable(wavetable);
  osc2.addWavetable(wavetable);
  osc3.addWavetable(wavetable);

  double freqVal = 20.0 / SAMPLE_RATE;
  double freqMult = 1.0 + (log(20000.0 / SAMPLE_RATE) - log(freqVal)) / numSamples;

  for (int i = 0; i < numSamples; i++)
  {
    osc.setFrequency(freqVal);
    data.recordedSamples[i] = osc.getSample();
    osc.update();
    freqVal *= freqMult;
  }
  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    Pa_Terminate();
    return 1;
  }
  outputParameters.channelCount = 1;                     /* stereo output */
  outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  printf("\n=== Now playing back. ===\n"); fflush(stdout);
  err = Pa_OpenStream(
      &stream,
      NULL, /* no input */
      &outputParameters,
      SAMPLE_RATE,
      FRAMES_PER_BUFFER,
      paClipOff,      /* we won't output out of range samples so don't bother clipping them */
      playCallback,
      &data );
  if( err != paNoError ) goto done;

  if( stream )
  {
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;

    printf("Waiting for playback to finish.\n"); fflush(stdout);

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
    if( err < 0 ) goto done;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

    printf("Done.\n"); fflush(stdout);
  }

done:
  Pa_Terminate();
  if( data.recordedSamples )       /* Sure it is NULL or valid. */
    free( data.recordedSamples );
  if( err != paNoError )
  {
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    err = 1;          /* Always return 0 or 1, but no other return codes. */
  }
  return err;
}

