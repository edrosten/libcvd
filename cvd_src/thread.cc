#include "cvd/thread.h"

std::atomic<unsigned int> CVD::Thread::ourCount;
thread_local CVD::Thread* CVD::Thread::current;

