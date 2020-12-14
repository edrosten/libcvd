#if defined(__ARM_NEON)
#include "NEON/half_sample.cc"
#elif (_M_IX86_FP >= 2) || defined(__SSE2__)
#include "SSE2/half_sample.cc"
#else
#include "noarch/half_sample.cc"
#endif
