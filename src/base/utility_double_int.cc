#if (_M_IX86_FP >= 2) || defined(__SSE2__)
#include "SSE2/utility_double_int.cc"
#else
#include "noarch/utility_double_int.cc"
#endif
