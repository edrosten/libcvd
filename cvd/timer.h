////////////////////////////////////////////////////////
// 
// A timer class designed for dealing with timestamps
// CK Nov 2002
//
////////////////////////////////////////////////////////

#ifndef __CVD_TIMER_H
#define __CVD_TIMER_H

namespace CVD {

class cvd_timer 
{
 public:
  cvd_timer() {reset();};
  double get_time();
  double conv_ntime(signed long long time);  // Conv from units of nanosecs
  void reset();                       // Sets startTime to the current time
 private:
  unsigned long long startTime;
};
 
 extern cvd_timer timer;
 
}

#endif
