#if (_M_IX86_FP >= 2) || defined(__SSE2__)
#include "SSE2/median_3x3.cc"
#else
#include "noarch/median_3x3.cc"
#endif
