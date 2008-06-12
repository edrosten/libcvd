#include <cvd/eventobject.h>
#ifdef WIN32
#include "Win32/win32.h"
#else
#include <sys/time.h>
#endif

namespace CVD {
EventObject::EventObject()
{
   pthread_cond_init(&myCondVar, 0);
}

EventObject::~EventObject()
{
   pthread_cond_destroy(&myCondVar);
}

void EventObject::trigger()
{
    pthread_cond_signal(&myCondVar);
}

void EventObject::triggerAll()
{
    pthread_cond_broadcast(&myCondVar);
}

void EventObject::wait()
{
    pthread_cond_wait(&myCondVar, &myMutex);
}

bool EventObject::wait(unsigned int milli)
{
   struct timeval tv;
   bool success = false;
   gettimeofday(&tv,0);
   struct timespec ts= {tv.tv_sec+milli/1000, tv.tv_usec*1000+(milli%1000)*1000000};
   int err = pthread_cond_timedwait(&myCondVar, &myMutex, &ts);
   if (err == 0)
       success = true;
   return success;
}
 
}
