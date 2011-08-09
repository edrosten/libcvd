#include <cvd/thread.h>
#include <sched.h>

namespace CVD{
	void Thread::yield()
	{
		sched_yield();
	}
}
