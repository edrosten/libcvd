#if (_M_IX86_FP >= 2) || defined(__SSE2__)
#include "SSE2/gradient.cc"
#else
#include "noarch/gradient.cc"
#endif
