///////////////////////////////////////////////////////
// 
// A timer class designed for dealing with timestamps
// CK Nov 2002
//
///////////////////////////////////////////////////////

#include "cvd/timer.h"
#include <iostream>

#include <sys/time.h>  //gettimeofday

namespace CVD {

void cvd_timer::reset() 
{
  struct timeval tv;

  gettimeofday(&tv,NULL);
  startTime=(unsigned long long)tv.tv_sec*1000000+tv.tv_usec;
}

double cvd_timer::get_time() 
{
  struct timeval tv;
  unsigned long long temp;  // Keep with integer arithmetic for diff

  gettimeofday(&tv,NULL);
  temp=(unsigned long long)tv.tv_sec*1000000+tv.tv_usec;

  return (temp-startTime)/1000000.0;
}

// Conv from units of nanosecs (specifically for v4l2)

double cvd_timer::conv_ntime(signed long long time)
{
  time=time/1000;  // now in us

  return (time-startTime)/1000000.0;
}

cvd_timer timer;

}
