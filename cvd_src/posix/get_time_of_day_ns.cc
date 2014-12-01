
#include "cvd/timer.h"
#include <inttypes.h>
#include <time.h>  //clock_gettime
#include <sys/time.h>  //timeval

namespace CVD {

long long get_time_of_day_ns()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (long long)ts.tv_sec*1000000000+ts.tv_nsec;
}

}
