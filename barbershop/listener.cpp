#include <cmath>
#include "listener.h"

int initDevices()
{
  return 4;
}

void currentValues(float *pitches, float *amplitudes)
{
  static int time = 0;

  time++;
  pitches[0] = sin(time/50.0)*110+155;
  pitches[1] = sin(time/55.0)*100+120;
  pitches[2] = sin(time/60.0)*80+105;
  pitches[3] = sin(time/65.0)*120+190;

  amplitudes[0] = sin(time/100.0)/2+0.5;
  amplitudes[1] = sin(time/50.0)/2+0.5;
  amplitudes[2] = sin(time/80.0)/2+0.5;
  amplitudes[3] = sin(time/120.0)/2+0.5;
}
