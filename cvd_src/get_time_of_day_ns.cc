
#include "cvd/timer.h"
#include <sys/time.h>  //gettimeofday

namespace CVD {

long long get_time_of_day_ns()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (long long)tv.tv_sec*1000000000+tv.tv_usec * 1000;
}

}
