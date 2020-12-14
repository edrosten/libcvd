#if (_M_IX86_FP >= 1) || defined(__SSE__)
#include "SSE/utility_float.cc"
#else
#include "noarch/utility_float.cc"
#endif
