// Win32 specific implementations of various POSIX functions not supported in Visual Studio

#ifndef CVD_INTERNAL_WIN32_H
#define CVD_INTERNAL_WIN32_H

#include <cvd/config.h>
#include <windows.h>

struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 
int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif  //CVD_INTERNAL_WIN32_H
