#include <cvd/synchronized.h>

namespace CVD {
pthread_mutexattr_t Synchronized::ourAttr;
bool Synchronized::ourInitFlag = false;

Synchronized::Synchronized() 
{
   if (!ourInitFlag) 
     {
	pthread_mutexattr_init(&ourAttr);
	pthread_mutexattr_settype(&ourAttr, PTHREAD_MUTEX_RECURSIVE);
	ourInitFlag = true;
     }
   pthread_mutex_init(&myMutex, &ourAttr);
}

Synchronized::~Synchronized() 
{
   lock();
   pthread_mutex_destroy(&myMutex);
}

void Synchronized::lock() const
{
   pthread_mutex_lock(&myMutex);
}

void Synchronized::unlock() const
{
   pthread_mutex_unlock(&myMutex);
}

}
