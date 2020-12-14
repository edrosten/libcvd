#if (_M_IX86_FP > 0) || defined(__SSE__)
#include "SSE/convolve_gaussian.cc"
#else
#include "noarch/convolve_gaussian.cc"
#endif
