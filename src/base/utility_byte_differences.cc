#if (_M_IX86_FP >= 0) || defined(__MMX__)
#include "MMX/utility_byte_differences.cc"
#else
#include "noarch/utility_byte_differences.cc"
#endif
