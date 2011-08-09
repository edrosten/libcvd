#include <cvd/thread.h>
#include <pthread.h>

namespace CVD{
	void Thread::yield()
	{
		pthread_yield_np();
	}
}
