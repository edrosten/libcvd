#if defined(_M_IX86) || defined(__MMX__)
#include "MMX/utility_byte_differences.cc"
#else
#include "noarch/utility_byte_differences.cc"
#endif
