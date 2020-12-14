#if (_M_IX86_FP >= 2) || defined(__SSE2__)
#include "SSE2/two_thirds_sample.cc"
#else
#include "noarch/two_thirds_sample.cc"
#endif
