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

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.4
double cvd_timer::conv_ntime(signed long long time)
{
  time=time/1000;  // now in us

  return (time-startTime)/1000000.0;
}

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.6
// Let the overloading mecnanism sort out the difference in the 
// headers from 2.4 hee, hee, hee
double cvd_timer::conv_ntime(const struct timeval& tv)
{
	double time = tv.tv_sec + tv.tv_usec*1e-6;
	return time-startTime / 1e6;
}

cvd_timer timer;

}
